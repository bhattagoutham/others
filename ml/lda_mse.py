import matplotlib.pyplot as plt
import numpy as np
from numpy.linalg import inv
from numpy.linalg import eig

d2_c1 = np.array([[3,3,1], [3,0,1], [2,1,1], [0,1.5,1]])
d2_c2 = np.array([[-1,1,1], [0,0,1], [-1,-1,1],[1,0,1]])

d1_c1 = np.array([[3,3,1], [3,0,1], [2,1,1], [0,2,1]])
d1_c2 = np.array([[-1,1,1], [0,0,1], [-1,-1,1],[1,0,1]])


def plot_1d(c1, c2, w, plt_nm):
	
	c1_y = np.zeros(len(c1))
	c2_y = np.zeros(len(c2))
	plt.show()
	plt.plot(c1,c1_y,'go', label='class 1')
	plt.plot(c2,c2_y,'ro', label='class 2')
	plt.plot(w, 0, 'bo',label='perceptron threshold')
	plt.suptitle(plt_nm)
	plt.legend(loc='upper right')
	plt.show()


def plot_data(d1_c1, d1_c2, res_1, res_2, plt_nm):
	
	plt.plot(d1_c1[:,0],d1_c1[:,1], 'go', label='class 1')
	plt.plot(d1_c2[:,0],d1_c2[:,1], 'ro', label='class 2')
	m_1 = (res_1[0][1]/res_1[0][0])
	m_2 = -(res_2[0][1]/res_2[0][0])
	x_2 = np.linspace(-10,10)
	y = m_1*x_2
	x_1 = m_2*x_2 - (res_2[0][2]/res_2[0][1])
	plt.plot(x_2, y,color='blue', label='Fishers projection vector')
	plt.plot(x_1,x_2, color='black', label='Least square classifier')
	plt.xlabel('x1')
	plt.ylabel('x2')
	plt.axis([-4, 4, -4, 4])
	plt.suptitle(plt_nm)
	plt.legend(loc='upper left')
	plt.show()



def fishers_lda(d_c1, d_c2, plt_nm):
	
	c1 = d_c1[:,0:2]
	c2 = d_c2[:,0:2]

	t = c1 - np.mean(c1, axis=0)
	s_1 = np.dot(np.transpose(t) , t)

	t = c2 - np.mean(c2, axis=0)
	s_2 = np.dot(np.transpose(t) , t)

	s_w = s_1 + s_2
	
	m = np.mean(np.vstack((c1,c2)), axis=0)
	
	m_1 = (np.mean(c1, axis=0) - m)
	m_2 = (np.mean(c2, axis=0) - m)

	res = [np.dot(inv(s_w),(m_1-m_2))]

	# project to 1D
	c1_1d = np.dot(c1, np.transpose(res))
	c2_1d = np.dot(c2, np.transpose(res))

	train_x = np.vstack((c1_1d, c2_1d))
	temp = np.transpose([np.ones(len(c1_1d))])
	train_y = np.vstack((temp,temp*(-1)))

	# apply perceptron to 1D data
	w = vannila_perceptron_train(train_x, train_y, 1, 0.3)
	
	plot_1d(c1_1d, c2_1d, w, plt_nm)

	return res


def least_square(c1, c2):

	x = np.vstack((c1, c2))
	temp = np.transpose([np.ones(len(c1))])
	y = np.vstack((temp,temp*(-1)))
	x_t = np.transpose(x)
	t1 = np.dot(x_t, x)
	t1 = inv(t1)
	t2 = np.dot(t1, x_t)
	w = np.dot(t2, y)

	return np.transpose(w)


def vannila_perceptron_train(train_x, train_y, epochs, alpha):

	
	(r,c) = train_x.shape
	data = train_x
	y = train_y
	W = np.zeros((c, 1))
	
	
	for iters in range(epochs):
		mis=0
		i=0
		for x in data:
			x = x.astype(float)
			y_t = float(y[i][0])
			hyp = np.matmul(x, W)

			if np.dot(y_t,hyp) <= 0:
				# wrong classification
				x = np.transpose([x])
				W = W + ((alpha*y_t) * x)
				mis+=1
			else:
				# correct classification
				pass
			i+=1

		#print(mis, alpha, np.transpose(W))
	return W


res_1 = fishers_lda(d1_c1, d1_c2, 'Dataset 1')
res_2 = least_square(d1_c1, d1_c2)
print(res_1)
#print(res_2)
plot_data(d1_c1, d1_c2, res_1, res_2, 'Dataset 1')


res_1 = fishers_lda(d2_c1, d2_c2, 'Dataset 2')
res_2 = least_square(d2_c1, d2_c2)
plot_data(d2_c1, d2_c2, res_1, res_2, 'Dataset 2')
