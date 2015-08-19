// Foveate Scene
//
// Copyright (C) 2008
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Fri Feb 29 15:26:29 CST 2008

#ifndef FOVEATE_SCENE_H
#define FOVEATE_SCENE_H

#include "foveator.h"
#include "foveate_image_conversion.h"
#include <climits>
#include <fstream>
#include <iterator>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QImage>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPoint>
#include <QStatusBar>
#include <QTime>
#include <iostream>

namespace horny_toad
{

class FoveateScene : public QGraphicsScene
{
    Q_OBJECT

    public:
    FoveateScene ()
        : statusbar (0)
        , coord_label (0)
        , framerate_label (0)
        , image_info_label (0)
        , compression_label (0)
        , foveation_label (0)
        , blending_map_ptr (0)
        , fixation_x (0)
        , fixation_y (0)
        , current_x (0)
        , current_y (0)
        , fixation_mode (FixationModeContinuous)
        , foveation_mode (FoveationModeFunction)
        , wheel_mode (WheelModeE2)
        , background_color (QColor (255, 255, 255))
        , scale (1)
        , foveate_y_channel (1)
        , foveate_u_channel (1)
        , foveate_v_channel (1)
    {
        e2_blending_map_object.set_e2 (25);
        bitmap_blending_map_object.set_divisor (BITMAP_BLENDING_MAP_DIVISOR);
        blending_map_ptr = &e2_blending_map_object;
        fov1.set_blending_map (blending_map_ptr);
        fov2.set_blending_map (blending_map_ptr);
        fov3.set_blending_map (blending_map_ptr);
        QImage tmp (320, 240, QImage::Format_RGB32);
        tmp.fill (~0); // white
        setImage (tmp);
    }
    static const unsigned BITMAP_BLENDING_MAP_DIVISOR = 8;
    const QImage &getDestinationImage ()
    { return dest; }
    void setStatusBar (QStatusBar *s)
    { statusbar = s; }
    void setCoordLabel (QLabel *c)
    { coord_label = c; }
    void setFrameRateLabel (QLabel *f)
    { framerate_label = f; }
    void setImageInfoLabel (QLabel *i)
    { image_info_label = i; }
    void setCompressionLabel (QLabel *c)
    { compression_label = c; }
    void setFoveationLabel (QLabel *f)
    { foveation_label = f; }
    void setImage (const QImage &image)
    {
        // Save original
        source = image;
        // Put fixation in the center
        fixation_x = static_cast<int> (source.width () / 2);
        fixation_y = static_cast<int> (source.height () / 2);
        const size_t W = source.width ();
        const size_t H = source.height ();
        // Load foveators with new image
        switch (source.format ())
        {
            default:
                throw std::runtime_error ("Unsupported image format");
            case QImage::Format_ARGB32:
            case QImage::Format_RGB32:
            {
                jack_rabbit::raster<unsigned char> m1 (H, W);
                jack_rabbit::raster<unsigned char> m2 (H, W);
                jack_rabbit::raster<unsigned char> m3 (H, W);
                load_yuv (source, m1, m2, m3);
                fov1.resize (H, W);
                fov1.set_source_pixels (m1.begin (), m1.end ());
                //fov1.preload (5); //tmp progressive foveation
                fov2.resize (H, W);
                fov2.set_source_pixels (m2.begin (), m2.end ());
                //fov2.preload (5); //tmp progressive foveation
                fov3.resize (H, W);
                fov3.set_source_pixels (m3.begin (), m3.end ());
                //fov3.preload (5); //tmp progressive foveation
            }
            break;
            case QImage::Format_Indexed8:
            {
                jack_rabbit::raster<unsigned char> m1 (H, W);
                load_grayscale (source, m1);
                fov1.resize (H, W);
                fov1.set_source_pixels (m1.begin (), m1.end ());
                fov2.resize (0, 0);
                fov3.resize (0, 0);
            }
            break;
        }
        // Set the new rect
        setSceneRect (0, 0, source.width () * scale, source.height () * scale);
        // Load with the unfoveated image
        dest = source;
        // Show image stats
        image_info (QString ("%1X%2 ").arg (image.width ()).arg (image.height ())
                + QString (image.format () == QImage::Format_Indexed8 ? "gs" : "rgb")
                + QString (" pixels"));
        encode ();
    }
    void setResolutionBitmap (const QImage &image)
    {
        const size_t W = image.width ();
        const size_t H = image.height ();
        // Load resolution bitmap
        switch (image.format ())
        {
            default:
                throw std::runtime_error ("Unsupported image format");
            case QImage::Format_ARGB32:
            case QImage::Format_RGB32:
            {
                jack_rabbit::raster<unsigned char> m1 (H, W);
                jack_rabbit::raster<unsigned char> m2 (H, W);
                jack_rabbit::raster<unsigned char> m3 (H, W);
                load_yuv (image, m1, m2, m3);
                // Set it to the Y channel
                bitmap_blending_map_object.set_bitmap (m1);
            }
            break;
            case QImage::Format_Indexed8:
            {
                jack_rabbit::raster<unsigned char> m (H, W);
                load_grayscale (image, m);
                bitmap_blending_map_object.set_bitmap (m);
            }
            break;
        }
    }
    float getE2 () const
    { return e2_blending_map_object.get_e2 (); }
    void setE2 (float e2)
    { e2_blending_map_object.set_e2 (e2); }
    int getFixationMode () const
    { return static_cast<int> (fixation_mode); }
    void setFixationMode (int mode)
    {
        QString s ("Fixation mode set to ");
        switch (mode)
        {
            default:
            throw std::runtime_error ("Unknown fixation mode");
            case FixationModeSingle:
            fixation_mode = FixationModeSingle;
            s += "Single";
            break;
            case FixationModeContinuous:
            fixation_mode = FixationModeContinuous;
            fixation_x = current_x;
            fixation_y = current_y;
            s += "Continuous";
            break;
        }
        msg (s);
        encode ();
    }
    int getFoveationMode () const
    { return static_cast<int> (foveation_mode); }
    void setFoveationMode (int mode)
    {
        QString s ("Foveation mode set to ");
        switch (mode)
        {
            default:
            throw std::runtime_error ("Unknown foveation mode");
            case FoveationModeFunction:
            blending_map_ptr = &e2_blending_map_object;
            fov1.set_blending_map (blending_map_ptr);
            fov2.set_blending_map (blending_map_ptr);
            fov3.set_blending_map (blending_map_ptr);
            foveation_mode = FoveationModeFunction;
            s += "Function";
            break;
            case FoveationModeBitmap:
            blending_map_ptr = &bitmap_blending_map_object;
            fov1.set_blending_map (blending_map_ptr);
            fov2.set_blending_map (blending_map_ptr);
            fov3.set_blending_map (blending_map_ptr);
            {
            for (size_t i = 0; i < 5; ++i)
            {
            std::vector<rect<float> > b =
                blending_map_ptr->get_blending_regions (i);
            for (size_t j = 0; j < b.size (); ++j)
                std::cout << i << ": "
                    << b[j].get_x () << ","
                    << b[j].get_y () << ", "
                    << b[j].get_width () << ", "
                    << b[j].get_height () << std::endl;
            }
            }
            foveation_mode = FoveationModeBitmap;
            s += "Bitmap";
            break;
            case FoveationModeOff:
            foveation_mode = FoveationModeOff;
            s += "Off";
            break;
        }
        fov1.set_blending_map (blending_map_ptr);
        fov2.set_blending_map (blending_map_ptr);
        fov3.set_blending_map (blending_map_ptr);
        msg (s);
        encode ();
    }
    int getWheelMode () const
    { return static_cast<int> (wheel_mode); }
    void setWheelMode (int mode)
    {
        QString s ("Wheel mode set to ");
        switch (mode)
        {
            default:
            throw std::runtime_error ("Unknown wheel mode");
            case WheelModeE2:
            wheel_mode = WheelModeE2;
            s += "Falloff";
            break;
            case WheelModeOff:
            wheel_mode = WheelModeOff;
            s += "Off";
            break;
        }
        msg (s);
    }
    QColor getBackgroundColor () const
    { return background_color; }
    void setBackgroundColor (QColor c)
    {
        background_color = c;
        invalidate (QRectF (INT_MIN/2, INT_MIN/2, INT_MAX, INT_MAX));
    }
    float getScale () const
    { return scale; }
    void setScale (float s)
    {
        scale = s;
        setSceneRect (0, 0, source.width () * scale, source.height () * scale);
        invalidate ();
    }
    bool getFoveateYChannel () const
    { return foveate_y_channel; }
    void setFoveateYChannel (bool flag)
    {
        foveate_y_channel = flag;
        QString s ("Luminance channel foveation set to ");
        s += (flag ? "ON" : "OFF");
        msg (s);
        encode ();
    }
    bool getFoveateUChannel () const
    { return foveate_u_channel; }
    void setFoveateUChannel (bool flag)
    {
        foveate_u_channel = flag;
        QString s ("Color channel foveation set to ");
        s += (flag ? "ON" : "OFF");
        msg (s);
        encode ();
    }
    bool getFoveateVChannel () const
    { return foveate_v_channel; }
    void setFoveateVChannel (bool flag)
    {
        foveate_v_channel = flag;
        QString s ("Color channel foveation set to ");
        s += (flag ? "ON" : "OFF");
        msg (s);
        encode ();
    }
    void drawBackground (QPainter *painter, const QRectF &rect)
    {
        painter->fillRect (rect, background_color);
        QRect tmp (QRect (QPoint (0, 0),
            QPoint (static_cast<int> (dest.width () * scale),
                static_cast<int> (dest.height () * scale))));
        painter->drawImage (tmp, dest);
    }
    void mousePressEvent (QGraphicsSceneMouseEvent *event)
    {
        QPointF p = event->buttonDownScenePos (Qt::LeftButton);
        current_x = static_cast<int> (p.x () / scale);
        current_y = static_cast<int> (p.y () / scale);
        if (fixation_mode == FixationModeSingle)
        {
            fixation_x = current_x;
            fixation_y = current_y;
            encode ();
        }
    }
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *event)
    {
        Q_UNUSED (event);
    }
    void mouseMoveEvent (QGraphicsSceneMouseEvent *event)
    {
        QPointF p = event->scenePos ();
        current_x = static_cast<int> (p.x () / scale);
        current_y = static_cast<int> (p.y () / scale);
        coord (QString ("[%1,%2]").arg (current_x).arg (current_y));
        // shift disables mouse updating
        if ((!(QApplication::keyboardModifiers () & Qt::ShiftModifier))
            && (fixation_mode == FixationModeContinuous))
        {
            fixation_x = current_x;
            fixation_y = current_y;
            encode ();
        }
    }
    void mouseDoubleClickEvent (QGraphicsSceneMouseEvent *event)
    {
        Q_UNUSED (event);
    }
    void wheelEvent (QGraphicsSceneWheelEvent *event)
    {
        // Don't pass the event on to the parent
        event->accept ();
        int degrees = event->delta () / 8;
        switch (wheel_mode)
        {
            case WheelModeE2:
            {
                float e2 = e2_blending_map_object.get_e2 ();
                if (e2 < 25)
                { if (degrees > 0) e2 += 1; else e2 -= 1; }
                else if (e2 < 125)
                { if (degrees > 0) e2 += 5; else e2 -= 5; }
                else
                { if (degrees > 0) e2 += 25; else e2 -= 25; }
                if (e2 < 5) e2 = 5;
                if (e2 > 100) e2 = 100;
                e2_blending_map_object.set_e2 (e2);
                encode ();
            }
            break;
            case WheelModeOff:
            default:
            break;
        }
    }
    enum FixationMode
    {
        FixationModeContinuous,
        FixationModeSingle
    };
    enum FoveationMode
    {
        FoveationModeBitmap,
        FoveationModeFunction,
        FoveationModeOff
    };
    enum WheelMode
    {
        WheelModeE2,
        WheelModeOff
    };
    private slots:
    void encode ()
    {
        switch (foveation_mode)
        {
            case FoveationModeOff:
            dest = source;
            compression (QString ("compression=%1%").arg (0.0, 0, 'f', 1));
            invalidate ();
            return;
            case FoveationModeFunction:
            break;
            case FoveationModeBitmap:
            break;
            default:
            throw std::runtime_error ("Unknown foveation mode");
        }
        try
        {
            float uncompressed_bytes = 0;
            float compressed_bytes = 0;
            QTime t;
            t.start ();
            blending_map_ptr->set_fixation (fixation_x, fixation_y);
            switch (dest.format ())
            {
                default:
                    throw std::runtime_error ("Unsupported image format");
                case QImage::Format_Indexed8:
                {
                    if (foveate_y_channel)
                    {
                        uncompressed_bytes += source.width () * source.height ();
                        compressed_bytes += fov1.foveate ();
                        unload_grayscale (fov1.get_foveated_pyramid ()[0],
                            dest);
                    }
                    else
                    {
                        compressed_bytes += source.width () * source.height ();
                        uncompressed_bytes += source.width () * source.height ();
                        unload_grayscale (fov1.get_source_pyramid ()[0],
                            dest);
                    }
                }
                break;
                case QImage::Format_ARGB32:
                case QImage::Format_RGB32:
                {
                    const jack_rabbit::raster<unsigned char> *m1;
                    const jack_rabbit::raster<unsigned char> *m2;
                    const jack_rabbit::raster<unsigned char> *m3;

                    if (foveate_y_channel)
                    {
                        compressed_bytes += fov1.foveate ();
                        // tmp progressive compressed_bytes += fov1.foveate (true);
                        uncompressed_bytes += source.width () * source.height ();
                        m1 = &fov1.get_foveated_pyramid ()[0];
                    }
                    else
                    {
                        compressed_bytes += source.width () * source.height ();
                        uncompressed_bytes += source.width () * source.height ();
                        m1 = &fov1.get_source_pyramid ()[0];
                    }

                    if (foveate_u_channel)
                    {
                        compressed_bytes += fov2.foveate ();
                        // tmp progressive compressed_bytes += fov2.foveate (true);
                        uncompressed_bytes += source.width () * source.height ();
                        m2 = &fov2.get_foveated_pyramid ()[0];
                    }
                    else
                    {
                        compressed_bytes += source.width () * source.height ();
                        uncompressed_bytes += source.width () * source.height ();
                        m2 = &fov2.get_source_pyramid ()[0];
                    }

                    if (foveate_v_channel)
                    {
                        compressed_bytes += fov3.foveate ();
                        // tmp progressive compressed_bytes += fov3.foveate (true);
                        uncompressed_bytes += source.width () * source.height ();
                        m3 = &fov3.get_foveated_pyramid ()[0];
                    }
                    else
                    {
                        compressed_bytes += source.width () * source.height ();
                        uncompressed_bytes += source.width () * source.height ();
                        m3 = &fov3.get_source_pyramid ()[0];
                    }

                    unload_yuv (*m1, *m2, *m3, dest);
                }
                break;
            }
            int elapsed = (std::max) (t.elapsed (), 1);
            float fps = 1000.0 / elapsed;
            framerate (QString ("%1 fps").arg (fps, 0, 'f', 1));
            float compression_rate = compressed_bytes / uncompressed_bytes;
            compression (QString ("%1:1 compression").arg (1 / compression_rate, 0, 'f', 1));
            foveation (QString ("e2=%1").arg (e2_blending_map_object.get_e2 ()));
        }
        catch (const std::exception &e)
        {
            msg (QString ("Exception: %1").arg (e.what ()));
            std::cerr << "Exception: " << e.what () << std::endl;
            QMessageBox::critical (0, QString ("Exception"), QString (e.what ()));
            return;
        }
        invalidate (0, 0, dest.width () * scale, dest.height () * scale);
    }
    void msg (const QString &text)
    {
        if (statusbar)
            statusbar->showMessage (text);
    }
    void coord (const QString &text)
    {
        if (coord_label)
            coord_label->setText (text);
    }
    void framerate (const QString &text)
    {
        if (framerate_label)
            framerate_label->setText (text);
    }
    void image_info (const QString &text)
    {
        if (image_info_label)
            image_info_label->setText (text);
    }
    void compression (const QString &text)
    {
        if (compression_label)
            compression_label->setText (text);
    }
    void foveation (const QString &text)
    {
        if (foveation_label)
            foveation_label->setText (text);
    }
    private:
    QStatusBar *statusbar;
    QLabel *coord_label;
    QLabel *framerate_label;
    QLabel *image_info_label;
    QLabel *compression_label;
    QLabel *foveation_label;
    blending_map<float> *blending_map_ptr;
    e2_blending_map<float> e2_blending_map_object;
    bitmap_blending_map<jack_rabbit::raster<unsigned char>, float> bitmap_blending_map_object;
    typedef foveator<unsigned char,kernel2x2_with_blur> Foveator;
    Foveator fov1;
    Foveator fov2;
    Foveator fov3;
    int fixation_x, fixation_y;
    int current_x, current_y;
    FixationMode fixation_mode;
    FoveationMode foveation_mode;
    WheelMode wheel_mode;
    QImage source;
    QImage dest;
    QColor background_color;
    float scale;
    bool foveate_y_channel;
    bool foveate_u_channel;
    bool foveate_v_channel;
};

} // namespace horny_toad

#endif // FOVEATE_SCENE_H
