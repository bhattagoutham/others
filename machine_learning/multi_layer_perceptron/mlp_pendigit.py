import numpy as np
import csv
import os

net_weights = {}

def loadDS(file_name):

	data = np.array([[]])
	y = np.array([])

	if not os.path.exists(file_name):
		print("******Invalid file name********")
		sys.exit(1)

	with open(file_name,'r') as f:

		reader = csv.reader(f)
		for row in reader:

			if "?" in row:
				continue

			x = row[0:-1]
			# x.append('1')
			x = list(map(int, x))

			if data.shape == (1, 0):
				data = np.hstack((data, [x]))
			else:
				data = np.vstack((data, [x]))
					
			y = np.hstack((y, int(row[-1])))
	
		(r,) = y.shape
		y = np.reshape(y, (r,1))
	# print(data.shape, y.shape)
	return (data, y)



def add_layer(no_of_inp_nodes, no_of_out_nodes, layer_no):
	global net_weights
	net_weights[layer_no] = np.asfarray(np.random.rand(no_of_inp_nodes, no_of_out_nodes)/ np.sqrt(no_of_inp_nodes) )

def create_net(no_of_inp_nodes, no_of_out_nodes, nh):
	add_layer(no_of_inp_nodes, no_of_inp_nodes+nh,1)
	add_layer(no_of_inp_nodes+nh, no_of_out_nodes,2)


def act_fun(inp, select):
	
	if select == "sigmoid":	
		tmp = np.exp(inp)
		return tmp/(1+tmp)

	elif select == "tanh":
		tmp = np.tanh(inp)
		return tmp

def loss_fun(prob, y):
	# cross entropy loss
	return prob - y


def softmax(hyp):
	# print hyp
	temp = np.zeros(hyp.shape)
	i=0
	for x in hyp:
		temp[i] = np.exp(x)/(sum(np.exp(x)))
		i+=1
	return temp

def derivative_of_act_fun(a, select):
	if select == "sigmoid":	
		return (a - np.power(a, 2)) 

	elif select == "tanh":
		return (1 - np.power(a, 2)) 



def test_acc(X, y, act_fun_val):
	z1 = np.dot(X, net_weights[1])
	a1 = act_fun(z1, act_fun_val)
	z2 = np.dot(a1, net_weights[2])
	prob = softmax(z2)
	# print prob.shape, y.shape
	corr = 0.0
	for i in range(prob.shape[0]):
		idx, = np.where(prob[i] == np.max(prob[i]))
		idx_v = idx[0]
		if int(y[i][idx_v]) == 1:
			corr += 1

	print "test_acc: ", (corr/prob.shape[0])*100

def cost_of_net(X, y, act_fun_val):
	
	z1 = np.dot(X, net_weights[1])
	a1 = act_fun(z1, act_fun_val)
	z2 = np.dot(a1, net_weights[2])
	prob = softmax(z2)
	temp = -np.log(prob)
	temp2 = np.multiply(temp, y)
	print np.sum(temp2)/y.shape[0]

	for i in range(prob.shape[0]):
		temp[i] = (prob[i] - y[i])




def forward_and_back_prop(X, y, alpha, act_fun_val):
	
	b1 = np.zeros((1, net_weights[2].shape[0])) 
	b2 = np.zeros((1, net_weights[2].shape[1])) 

	z1 = np.dot(X, net_weights[1]) + b1
	a1 = act_fun(z1, act_fun_val)
	z2 = np.dot(a1, net_weights[2]) + b2
	prob = softmax(z2)
	
	del3 = loss_fun(prob, train_y)
	dW2 = np.dot(np.transpose(a1), del3)
	db2 = np.sum(del3, axis=0, keepdims=True) 
	del2 = del3.dot(np.transpose(net_weights[2])) * derivative_of_act_fun(a1, act_fun_val)
	dW1 = np.dot(np.transpose(X), del2)
	db1 = np.sum(del2, axis=0) 
	
	# dW1 = dW1 + (0.001 * net_weights[1])
	# dW2 = dW2 + (0.001 * net_weights[2])

	net_weights[1] = net_weights[1] - (alpha * np.asfarray(dW1))
	net_weights[2] = net_weights[2] - (alpha * np.asfarray(dW2))
	b1 = b1 - (alpha * db1)
	b2 = b2 - (alpha * db2)	

def train_network(train_data, train_y, epochs, act):
	
	for iter in range(epochs):
			forward_and_back_prop(train_data, train_y, 0.0001, act)
			if iter % 100 == 0:
				print "Iter: ", iter, " Cost: ",
				cost_of_net(train_data, train_y,act)

def convert_to_y_hot(y):
	y_hot = np.zeros((y.shape[0], 10))
	i=0
	for x in y:
		y_hot[i][int(x)-1] = 1
		# print (x, y_hot[i])
		i+=1
	return y_hot

act = "sigmoid"
nh = 20

(data, y) = loadDS("pendigits.tra")
n, no_of_features = data.shape

print(act, no_of_features+nh)

out_class = list(set(y[:,0]))
no_of_out_nodes = len(out_class)
m =  np.mean(data, axis = 0)
s = np.std(data,  axis = 0)
data = (data - m)/s
print "train_data: ", data.shape
split = 0.6; epochs = 1000;
train_data = data
# train_data_size = int(mt.floor(n*split))
# train_data = data[0:train_data_size,:]
# test_data = data[train_data_size:n,:]
train_y = convert_to_y_hot(y)

create_net(no_of_features, 10, nh)
train_network(train_data, train_y, epochs, act)

(data, y) = loadDS("pendigits.tes")
print "test_data: ", data.shape
m =  np.mean(data, axis = 0)
s = np.std(data,  axis = 0)
data = (data - m)/s
test_data = data
test_y = convert_to_y_hot(y)
test_acc(test_data, test_y, act)