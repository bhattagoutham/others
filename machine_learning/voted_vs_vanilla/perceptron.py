import numpy as np
import csv, sys, os
import matplotlib.pyplot as plt

def checkforQ(list):
	
	for i in list:
		if i == '?':
			return 1
	return 0

def loadDS(file_name):

	data = np.array([[]])
	y = np.array([])
	strt = 0
	if file_name.find('cancer') != -1:
		strt = 1

	if not os.path.exists(file_name):
		print("******Invalid file name********")
		sys.exit(1)


	with open(file_name,'r') as f:

		reader = csv.reader(f)
		for row in reader:

			if checkforQ(row):
				pass
			else:
				x = row[strt:-2]
				x.append('1')

				if data.shape == (1, 0):
					data = np.hstack((data, [x]))
				else:
					data = np.vstack((data, [x]))
				
				if strt == 0:
					if row[-1] == 'g':
						val = 1
					else:
						val = -1
					y = np.hstack((y, val))
				else:
					if row[10] == '2':
						val = 1
					else:
						val = -1
					y = np.hstack((y, val))

		
		(r,) = y.shape
		y = np.reshape(y, (r,1))
	
	f.close()
	return (data, y)

def plot_graph(file_name, epochs, van_acc, vot_acc):
	van_acc = van_acc*100
	vot_acc = vot_acc*100
	plt.plot(epochs, vot_acc, label='voted perceptron', color='green')
	plt.plot(epochs, van_acc, label='vanilla perceptron', color='red')
	plt.axis([10, 60, 80, 100])
	plt.xlabel('epochs')
	plt.ylabel('accuracy (%)')
	plt.suptitle(file_name)
	plt.legend(loc='upper right')
	plt.show()

    
def run_vannila_perceptron(file_name):

	kfold = 10
	epochs = [10, 15, 20, 25, 30, 35, 40, 45, 50]
	alpha = 1
	acc = np.array([])

	print('vannila perceptron')
	print('step_size: ', alpha)
	print('epochs', '\t accuracy')
	
	for i in epochs:
		val = kfold_vannila_perceptron(file_name, kfold, i, alpha)
		acc = np.append(acc, val)
		print(i, '\t ',val)
	
	return acc
	
	
def kfold_vannila_perceptron(file_name, k, epochs, alpha):
	
	accuracy = np.array([])
	(data, y) = loadDS(file_name)
	(row, col) = data.shape; n=row;
	
	
	for i in range(k):

		fold = n/k
		strt = i*fold
		end = (i+1)*fold
		
		test_range = np.arange(strt, end, dtype=int)

		train_x = np.delete(data, test_range, 0)
		train_y = np.delete(y, test_range, 0)
		test_x = data[test_range]
		test_y = y[test_range]
		
		weights = vannila_perceptron_train(train_x, train_y, epochs, alpha)
		accuracy = np.append(accuracy, vannila_perceptron_test(weights, test_x, test_y))

	
	return np.average(accuracy)



def vannila_perceptron_test(weights, test_x, test_y):
	
	test_x = test_x.astype(float)
	hyp = np.matmul(test_x, weights)
	acc = hyp*test_y
	(r, c) = test_y.shape
	
	return (r-np.sum(acc <= 0))/float(r)



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


def run_voted_perceptron(file_name):

	kfold = 10
	epochs = [10, 15, 20, 25, 30, 35, 40, 45, 50]
	alpha = 1
	acc = np.array([])
	print('voted perceptron')
	print('step_size: ', alpha)
	print('epochs', '\t accuracy')
	
	for i in epochs:
		val = kfold_voted_perceptron(file_name, kfold, i, alpha)
		acc = np.append(acc, val)
		print(i, '\t ',val)
	
	return acc
	

def kfold_voted_perceptron(file_name, k, epochs, alpha):
	
	accuracy = np.array([])
	(data, y) = loadDS(file_name)
	(row, col) = data.shape; n=row;
	
	
	for i in range(k):

		fold = n/k
		strt = i*fold
		end = (i+1)*fold
		
		test_range = np.arange(strt, end, dtype=int)

		train_x = np.delete(data, test_range, 0)
		train_y = np.delete(y, test_range, 0)
		test_x = data[test_range]
		test_y = y[test_range]
		
		(weights, votes) = voted_perceptron_train(train_x, train_y, epochs, alpha)
		accuracy = np.append(accuracy, voted_perceptron_test(weights, votes, test_x, test_y))
		

	
	return np.average(accuracy)



def voted_perceptron_test(weights, votes, test_x, test_y):
	
	test_x = test_x.astype(float)
	misclassified = 0
	#print(weights.shape, votes.shape, test_x.shape)
	i=0
	for x in test_x:

		hyp_vec = np.matmul(weights,x)
		sign = np.dot(hyp_vec, votes)
		if test_y[i]*sign < 0:
			misclassified += 1
		i+=1

	(r, c) = test_y.shape
	return (r-misclassified)/float(r)



def voted_perceptron_train(train_x, train_y, epochs, alpha):

	(r,c) = train_x.shape
	data = train_x
	y = train_y
	curr_W = np.zeros((c, 1))
	weights = np.array([[]])
	votes = np.array([])
	cnt = 1

	for iters in range(epochs):
		i=0
		for x in data:
			x = x.astype(float)
			y_t = float(y[i][0])
			hyp = np.matmul(x, curr_W)

			if np.dot(y_t,hyp) <= 0:
				# wrong classification
				
				#store curr_W in weights
				if weights.shape == (1,0):
					weights = np.hstack((weights, np.transpose(curr_W)))
				else:
					weights = np.vstack((weights, np.transpose(curr_W)))

				#store cnt in votes
				votes = np.append(votes, cnt)

				#update weights
				x = np.transpose([x])
				curr_W = curr_W + ((alpha*y_t) * x)

				#reset vote
				cnt=1
			else:
				# correct classification

				#increase vote
				cnt+=1
			
			i+=1

		#print(mis, alpha, np.transpose(W))
	return (weights, votes)

try:
	file_name = sys.argv[1]
except IndexError:
	print('******Please enter file name eg: >>python perceptron.py ionosphere.data*********')
	sys.exit(1)

epochs = [10, 15, 20, 25, 30, 35, 40, 45, 50]
van_acc = run_vannila_perceptron(file_name)
vot_acc = run_voted_perceptron(file_name)
plot_graph(file_name, epochs, van_acc, vot_acc)



