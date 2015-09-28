import numpy as np

def sigmoid(z):
	"""
	Compute a sigmoid function
	"""
	return = 1.0 / (1.0 + np.exp(-z))

def sigmoid_grad(z):
	"""
	Compute the gradient of a sigmoid function
	"""
	sig_z = sigmoid(z)
	return sig_z * (1.0 - sig_z)

def cost_function(params, input_lyr_sz, hidden_lyr_sz, n_labels, X, y, lmbda):
	"""

	"""
	Theta1 = np.reshape(params[0:hidden_lyr_sz * (input_lyr_sz, + 1)], [hidden_lyr_sz, input_lyr_sz, + 1], order='F')
	Theta2 = np.reshape(params[hidden_lyr_sz * (input_lyr_sz + 1):], [n_labels, hidden_lyr_sz + 1], order='F')
	m = np.shape(X)[0]
	a1 = np.hstack((np.ones(m, 1), X))
	z2 = np.dot(a1, Theta1.T)
	a2 = sigmoid(z2)
	a2 = np.hstack((np.ones(m, 1), a2))
	z3 = np.dot(a2, Theta2.T)
	a3 = sigmoid(z3)
	ey = np.identity(n_labels)
	y = ey[y, :]
	reg_Theta1 = Theta1[:,1:]
	reg_Theta2 = Theta2[:,1:]
	reg = (lmbda / (2.0 * m)) * np.sum(np.ravel(reg_Theta1) ** 2) + np.sum(np.ravel(reg_Theta2 ** 2))
	J = (1.0 / m) + np.sum(np.sum(np.multiply(-y, np.log(a3)) - np.multiply(1 - y, np.log(1 - a3)))) + reg
	d3 = a3 - y
	d2 = np.multiply(np.dot(d3, Theta2[:,1:]), sigmoid_grad(z2))
	Theta1_grad = (1 / m) * (np.dot(d2.T, a1) + lmbda * Theta1)
	Theta2_grad = (1 / m) * (np.dot(d3.T, a2) + lmbda * Theta2)
	reg1 = np.multiply(lmbda / m, Theta1[:,0]) 
	reg2 = np.multiply(lmbda / m, Theta2[:,0])
	Theta1_grad[:,0] = Theta1_grad[:,0] - reg1;
	Theta2_grad[:,0] = Theta2_grad[:,0] - reg2;
	grad = np.vstack((np.ravel(Theta1_grad), np.ravel(Theta2_grad)))
	return J, grad

def predict(Theta1, Theta2, X):
	"""

	"""
	m = np.shape(X)[0]
	h1 = sigmoid(np.dot(np.hstack((np.ones(m, 1), X)), Theta1.T))
	h2 = sigmoid(np.dot(np.hstack((np.ones(m, 1), h1)), Theta2.T))
	return np.argmax(h2, axis=1)

def rand_init_weights(L_in, L_out, epsilon):
	"""

	"""
	return np.rand(L_out, 1 + L_in) * 2 * epsilon - epsilon
