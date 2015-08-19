// Foveate Main Window
//
// Copyright (C) 2008
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Feb 28 15:57:26 CST 2008

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "foveator.h"
#include "ui_foveate_export_dialog.h"
#include "ui_foveate_main_window.h"
#include "foveate_scene.h"
#include "foveate_view.h"
#include <QCloseEvent>
#include <QColor>
#include <QColorDialog>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QInputDialog>
#include <QMessageBox>
#include <QPalette>
#include <QProgressDialog>
#include <QSettings>
#include <QString>
#include <iostream>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow (QMainWindow *parent)
        : QMainWindow (parent)
        , settings ("cps.utexas.edu", "foveate")
        , open_directory (".")
        , save_directory (".")
        , source_filename ("")
        , blending_map_filename ("")
    {
        ui.setupUi(this);
        QActionGroup *fixationMode = new QActionGroup (this);
        fixationMode->addAction (ui.action_Fixation_Mode_Continuous);
        fixationMode->addAction (ui.action_Fixation_Mode_Single);
        QActionGroup *foveationMode = new QActionGroup (this);
        foveationMode->addAction (ui.action_Foveation_Mode_Function);
        foveationMode->addAction (ui.action_Foveation_Mode_Bitmap);
        foveationMode->addAction (ui.action_Foveation_Mode_Off);
        QActionGroup *wheelMode = new QActionGroup (this);
        wheelMode->addAction (ui.action_Wheel_Mode_Falloff);
        wheelMode->addAction (ui.action_Wheel_Mode_Off);
        readSettings ();
        QLabel *coord_label = new QLabel ();
        QLabel *framerate_label = new QLabel ();
        QLabel *compression_label = new QLabel ();
        QLabel *foveation_label = new QLabel ();
        QLabel *imageinfo_label = new QLabel ();
        ui.statusbar->addPermanentWidget (coord_label);
        ui.statusbar->addPermanentWidget (framerate_label);
        ui.statusbar->addPermanentWidget (compression_label);
        ui.statusbar->addPermanentWidget (foveation_label);
        ui.statusbar->addPermanentWidget (imageinfo_label);
        scene.setStatusBar (ui.statusbar);
        scene.setCoordLabel (coord_label);
        scene.setFrameRateLabel (framerate_label);
        scene.setCompressionLabel (compression_label);
        scene.setFoveationLabel (foveation_label);
        scene.setImageInfoLabel (imageinfo_label);
        ui.graphicsView->setScene (&scene);
        ui_export_dlg.setupUi (&export_dlg);
        setCursor ();
        // Read the source image if we can
        if (!source_filename.isEmpty ())
        {
            const QImage source_image = load (source_filename);
            if (!source_image.isNull ())
            {
                scene.setImage (source_image);
                setWindowTitle (source_filename);
            }
            else
            {
                source_filename = QString ("");
            }
        }
        // Read the blending map image if we can
        ui.action_Foveation_Mode_Bitmap->setEnabled (false);
        if (!blending_map_filename.isEmpty ())
        {
            const QImage blending_map_image = load (blending_map_filename);
            if (!blending_map_image.isNull ())
            {
                ui.action_Foveation_Mode_Bitmap->setEnabled (true);
                scene.setResolutionBitmap (blending_map_image);
            }
            else
            {
                blending_map_filename = QString ("");
                // Make sure the mode is not set to bitmap
                if (scene.getFoveationMode () == horny_toad::FoveateScene::FoveationModeBitmap)
                {
                    scene.setFoveationMode (horny_toad::FoveateScene::FoveationModeFunction);
                    ui.action_Foveation_Mode_Function->setChecked (true);
                    ui.action_Foveation_Mode_Bitmap->setChecked (false);
                }
            }
        }
        // Make the menubar actions visible, even when the
        // menubar is hidden
        this->addActions (ui.menubar->actions ());
        // Don't draw a frame around the graphics view
        ui.graphicsView->setFrameShape (QFrame::NoFrame);
        // Don't add a gray border around the graphics scene
        ui.gridLayout->setContentsMargins (0, 0, 0, 0);
    }

    protected:
    void closeEvent (QCloseEvent *event)
    {
        writeSettings();
        event->accept();
    }

    private slots:
    void on_action_Open_triggered ()
    {
        QString fn = QFileDialog::getOpenFileName (this,
            "Open",
            open_directory,
            tr("Images (*.bmp *.gif *.jpeg *.JPG *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm)"));
        if (fn.isNull ())
        {
            ui.statusbar->showMessage (QString ("User canceled"));
            return;
        }
        const QImage image = load (fn);
        if (!image.isNull ())
        {
            scene.setImage (image);
            setWindowTitle (fn);
            source_filename = fn;
        }
    }
    void on_action_Save_triggered ()
    {
        QString fn = QFileDialog::getSaveFileName (this,
            "Save",
            save_directory,
            tr("Images (*.bmp *.gif *.jpeg *.JPG *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm)"));
        if (fn.isNull ())
        {
            ui.statusbar->showMessage (QString ("User canceled"));
            return;
        }
        const QImage &image = scene.getDestinationImage ();
        save (image, fn);
    }
    void on_action_Export_Blending_Map_triggered ()
    {
        if (!export_dlg.exec ())
        {
            ui.statusbar->showMessage (QString ("User canceled"));
            return;
        }
        QString fn = QFileDialog::getSaveFileName (this,
            "Export",
            save_directory,
            tr("Images (*.bmp *.gif *.pbm *.pgm *.png *.ppm *.xbm *.xpm)"));
        if (fn.isNull ())
        {
            ui.statusbar->showMessage (QString ("User canceled"));
            return;
        }
        QImage im (ui_export_dlg.width->value (),
            ui_export_dlg.height->value (),
            QImage::Format_Indexed8);
        QVector<QRgb> t (256);
        for (int i = 0; i < t.size (); ++i)
            t[i] = QColor (i, i, i).rgb ();
        im.setColorTable (t);
        horny_toad::e2_blending_map<> map;
        map.set_e2 (ui_export_dlg.e2->value ());
        map.set_fixation (im.width () / 2.0, im.height () / 2.0);
        QProgressDialog progress ("Exporting...", "Abort", 0, im.height (), this);
        progress.setWindowModality (Qt::WindowModal);
        for (int y = 0; y < im.height (); ++y)
        {
            progress.setValue (y);
            if (progress.wasCanceled ())
            {
                ui.statusbar->showMessage (QString ("User canceled"));
                return;
            }
            float yoff = y;
            for (int x = 0; x < im.width (); ++x)
            {
                float xoff = x;
                float res = map.get_blending_level (xoff, yoff)
                    * horny_toad::FoveateScene::BITMAP_BLENDING_MAP_DIVISOR;
                if (res < 0)
                    res = 0;
                else if (res > 255)
                    res = 255;
                im.setPixel (x, y, static_cast<unsigned> (res));
            }
        }
        save (im, fn);
    }
    void on_action_Open_Blending_Map_triggered ()
    {
        QString fn = QFileDialog::getOpenFileName (this,
            "Open Blending Map",
            open_directory,
            tr("Images (*.bmp *.gif *.pbm *.pgm *.png *.ppm *.xbm *.xpm)"));
        if (fn.isNull ())
        {
            ui.statusbar->showMessage (QString ("User canceled"));
            return;
        }
        const QImage image = load (fn);
        if (!image.isNull ())
        {
            scene.setResolutionBitmap (image);
            blending_map_filename = fn;
            ui.action_Foveation_Mode_Bitmap->setEnabled (true);
        }
    }
    void on_action_Quit_triggered ()
    {
        close ();
    }
    void on_action_Fixation_Mode_Continuous_triggered ()
    {
        scene.setFixationMode (horny_toad::FoveateScene::FixationModeContinuous);
        setCursor ();
    }
    void on_action_Fixation_Mode_Single_triggered ()
    {
        scene.setFixationMode (horny_toad::FoveateScene::FixationModeSingle);
        setCursor ();
    }
    void on_action_Foveation_Mode_Function_triggered ()
    {
        scene.setFoveationMode (horny_toad::FoveateScene::FoveationModeFunction);
    }
    void on_action_Foveation_Mode_Bitmap_triggered ()
    {
        scene.setFoveationMode (horny_toad::FoveateScene::FoveationModeBitmap);
    }
    void on_action_Foveation_Mode_Off_triggered ()
    {
        scene.setFoveationMode (horny_toad::FoveateScene::FoveationModeOff);
    }
    void on_action_Luminance_Channel_triggered ()
    {
        scene.setFoveateYChannel (!scene.getFoveateYChannel ());
    }
    void on_action_Color_Channel_triggered ()
    {
        scene.setFoveateUChannel (!scene.getFoveateUChannel ());
        scene.setFoveateVChannel (!scene.getFoveateVChannel ());
    }
    void on_action_Wheel_Mode_Falloff_triggered ()
    {
        scene.setWheelMode (horny_toad::FoveateScene::WheelModeE2);
    }
    void on_action_Wheel_Mode_Off_triggered ()
    {
        scene.setWheelMode (horny_toad::FoveateScene::WheelModeOff);
    }
    void on_action_Statusbar_triggered ()
    {
        ui.statusbar->setVisible (!ui.statusbar->isVisible ());
    }
    void on_action_Menubar_triggered ()
    {
        ui.menubar->setVisible (!ui.menubar->isVisible ());
    }
    void on_action_Background_Color_triggered ()
    {
        QColor c = scene.getBackgroundColor ();
        c = QColorDialog::getColor (c);
        scene.setBackgroundColor (c);
    }
    void on_action_Scale_triggered ()
    {
        double scale = scene.getScale ();
        bool ok = false;
        scale = QInputDialog::getDouble (this,
            "Image Scale",
            "Scale (0.1 - 10.0)",
            scale,
            0.1,
            10.0,
            1,
            &ok);
        if (ok)
            scene.setScale (scale);
    }
    void on_action_Fullscreen_triggered ()
    {
        setWindowState (windowState() ^ Qt::WindowFullScreen);
    }
    void on_action_About_triggered ()
    {
        QMessageBox::about (this, "About", "Copyright 2002-2008\n"
            "Center for Perceptual Systems\n"
            "The University of Texas at Austin\n"
            "\n"
            "Contacts:\n"
            "jsp@mail.utexas.edu\n"
            "geisler@psy.utexas.edu");
    }

    private:
    void setCursor ()
    {
        switch (scene.getFixationMode ())
        {
            case horny_toad::FoveateScene::FixationModeContinuous:
            ui.graphicsView->viewport()->setProperty("cursor", QVariant(QCursor(Qt::CrossCursor)));
            break;
            case horny_toad::FoveateScene::FixationModeSingle:
            ui.graphicsView->viewport()->setProperty("cursor", QVariant(QCursor(Qt::ArrowCursor)));
            break;
            default:
            ui.graphicsView->viewport()->setProperty("cursor", QVariant(QCursor(Qt::ArrowCursor)));
        }
    }
    QImage load (const QString &fn)
    {
        ui.statusbar->showMessage (QString ("Opening ") + fn);
        QFileInfo file_info (fn);
        open_directory = file_info.path ();
        QPixmap pixmap;
        QImage empty;
        if (!pixmap.load (fn))
        {
            QString msg = QString ("Could not read image:\n") + QString (fn);
            QMessageBox::critical (this, QString ("Error"), msg);
            ui.statusbar->showMessage (msg);
            return empty;
        }
        QImage image = pixmap.toImage ();
        if (image.isNull ())
        {
            QString msg = QString ("Could not convert image:\n") + QString (fn);
            QMessageBox::critical (this, QString ("Error"), msg);
            ui.statusbar->showMessage (msg);
            return empty;
        }
        if (image.format () != QImage::Format_ARGB32 &&
            image.format () != QImage::Format_RGB32 &&
            image.format () != QImage::Format_Indexed8)
        {
            QString msg = QString ("Image format not supported:\n") + QString (fn);
            QMessageBox::critical (this, QString ("Error"), msg);
            ui.statusbar->showMessage (msg);
            return empty;
        }
        if (image.format () == QImage::Format_Indexed8 &&
            !image.isGrayscale ())
        {
            QString msg = QString ("Image format not supported:\n") + QString (fn);
            QMessageBox::critical (this, QString ("Error"), msg);
            ui.statusbar->showMessage (msg);
            return empty;
        }
        ui.statusbar->showMessage (QString ("Successfully opened ") + fn);
        return image;
    }
    void save (const QImage &image, const QString &fn)
    {
        ui.statusbar->showMessage (QString ("Writing ") + fn);
        if (!image.save (fn, 0, 100)) // 100=high quality
        {
            QString msg = QString ("Could not write file:\n") + QString (fn);
            QMessageBox::critical (this, QString ("Error"), msg);
            ui.statusbar->showMessage (msg);
            return;
        }
        ui.statusbar->showMessage (QString ("Successfully written ") + fn);
        QFileInfo file_info (fn);
        save_directory = file_info.path ();
    }
    void writeSettings()
    {
        settings.beginGroup ("File");
        settings.setValue ("open_directory", open_directory);
        settings.setValue ("save_directory", save_directory);
        settings.setValue ("source_filename", source_filename);
        settings.setValue ("blending_map_filename", blending_map_filename);
        settings.endGroup ();
        settings.beginGroup ("Options");
        settings.setValue ("fixation_mode", scene.getFixationMode ());
        settings.setValue ("foveation_mode", scene.getFoveationMode ());
        settings.setValue ("foveate_y_channel", scene.getFoveateYChannel ());
        settings.setValue ("foveate_u_channel", scene.getFoveateUChannel ());
        settings.setValue ("foveate_v_channel", scene.getFoveateVChannel ());
        settings.setValue ("e2", scene.getE2 ());
        settings.setValue ("wheel_mode", scene.getWheelMode ());
        settings.endGroup ();
        settings.beginGroup ("View");
        settings.setValue ("geometry", saveGeometry ());
        settings.setValue ("statusbar_flag", ui.statusbar->isVisible ());
        settings.setValue ("menubar_flag", ui.menubar->isVisible ());
        int r, g, b;
        scene.getBackgroundColor ().getRgb (&r, &g, &b);
        settings.setValue ("background_color_r", r);
        settings.setValue ("background_color_g", g);
        settings.setValue ("background_color_b", b);
        settings.setValue ("scale", scene.getScale ());
        settings.endGroup ();
    }
    void readSettings()
    {
        settings.beginGroup ("File");
        open_directory = settings.value ("open_directory", ".").toString ();
        save_directory = settings.value ("save_directory", ".").toString ();
        source_filename = settings.value ("source_filename", "").toString ();
        blending_map_filename = settings.value ("blending_map_filename", "").toString ();
        settings.endGroup ();
        settings.beginGroup ("Options");
        int fixation_mode = settings.value ("fixation_mode", 0).toInt ();
        switch (fixation_mode)
        {
            default:
            QMessageBox::warning (this,
                QString ("Warning"),
                QString ("Unknown fixation mode encountered in application settings"));
            fixation_mode = horny_toad::FoveateScene::FixationModeContinuous;
            // Fall through
            case horny_toad::FoveateScene::FixationModeContinuous:
            ui.action_Fixation_Mode_Continuous->setChecked (true);
            break;
            case horny_toad::FoveateScene::FixationModeSingle:
            ui.action_Fixation_Mode_Single->setChecked (true);
            break;
        }
        scene.setFixationMode (fixation_mode);
        int foveation_mode = settings.value ("foveation_mode", 0).toInt ();
        switch (foveation_mode)
        {
            default:
            QMessageBox::warning (this,
                QString ("Warning"),
                QString ("Unknown foveation mode encountered in application settings"));
            foveation_mode = horny_toad::FoveateScene::FoveationModeFunction;
            // Fall through
            case horny_toad::FoveateScene::FoveationModeFunction:
            ui.action_Foveation_Mode_Function->setChecked (true);
            break;
            case horny_toad::FoveateScene::FoveationModeBitmap:
            ui.action_Foveation_Mode_Bitmap->setChecked (true);
            break;
            case horny_toad::FoveateScene::FoveationModeOff:
            ui.action_Foveation_Mode_Off->setChecked (true);
            break;
        }
        scene.setFoveationMode (foveation_mode);
        bool flag;
        flag = settings.value ("foveate_y_channel", true).toBool ();
        scene.setFoveateYChannel (flag);
        ui.action_Luminance_Channel->setChecked (flag);
        flag = settings.value ("foveate_u_channel", true).toBool ();
        scene.setFoveateUChannel (flag);
        ui.action_Color_Channel->setChecked (flag);
        flag = settings.value ("foveate_v_channel", true).toBool ();
        scene.setFoveateVChannel (flag);
        ui.action_Color_Channel->setChecked (flag);
        double e2 = settings.value ("e2", -1.0).toDouble ();
        if (e2 > 0.0)
            scene.setE2 (e2);
        int wheel_mode = settings.value ("wheel_mode", 0).toInt ();
        switch (wheel_mode)
        {
            default:
            QMessageBox::warning (this,
                QString ("Warning"),
                QString ("Unknown wheel mode encountered in application settings"));
            wheel_mode = horny_toad::FoveateScene::WheelModeE2;
            // Fall through
            case horny_toad::FoveateScene::WheelModeE2:
            ui.action_Wheel_Mode_Falloff->setChecked (true);
            break;
            case horny_toad::FoveateScene::WheelModeOff:
            ui.action_Wheel_Mode_Off->setChecked (true);
            break;
        }
        scene.setWheelMode (wheel_mode);
        settings.endGroup ();
        settings.beginGroup ("View");
        restoreGeometry (settings.value ("geometry").toByteArray());
        if (isFullScreen ())
            ui.action_Fullscreen->setChecked (true);
        bool statusbar_flag = settings.value ("statusbar_flag", true).toBool ();
        ui.statusbar->setVisible (statusbar_flag);
        ui.action_Statusbar->setChecked (statusbar_flag);
        bool menubar_flag = settings.value ("menubar_flag", true).toBool ();
        ui.menubar->setVisible (menubar_flag);
        ui.action_Menubar->setChecked (menubar_flag);
        int r = settings.value ("background_color_r", 255).toInt ();
        int g = settings.value ("background_color_g", 255).toInt ();
        int b = settings.value ("background_color_b", 255).toInt ();
        scene.setBackgroundColor (QColor (r, g, b));
        float scale = settings.value ("scale", 1).toDouble ();
        scene.setScale (scale);
        settings.endGroup ();
    }
    Ui::MainWindow ui;
    Ui::ExportDialog ui_export_dlg;
    QDialog export_dlg;
    horny_toad::FoveateScene scene;
    QSettings settings;
    QString open_directory;
    QString save_directory;
    QString source_filename;
    QString blending_map_filename;
};

#endif
