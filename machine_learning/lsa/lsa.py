import os
import sys
# import nltk as nl
# from nltk.stem import PorterStemmer
import string
from collections import OrderedDict
import numpy as np
import math as mt
# from scipy.sparse import csc_matrix
# from scipy.sparse.linalg import svds, eigs

#nl.download('stopwords')

# porter = PorterStemmer()
# i = nl.corpus.stopwords.words('english')
j = list(string.punctuation)
# stopwords = set(i).union(j)
stopwords = set(j)
vocab_train = {}
vocab_test = {}

def out_class_dir_sort(dir_list):
	dir_names = os.listdir(dir_list)
	class_list = sorted(dir_names)
	return class_list

def in_class_dir_sort(dir_list, algo_check):

	dir_names = os.listdir(dir_list)
	class_list = sorted(dir_names)
	if algo_check == "train":
		class_list = class_list[0:int(0.8*len(class_list))]
	else:
		class_list = class_list[int(0.8*len(class_list)):]
	return class_list

def vocab_check(x, algo_check):
	global vocab_train, vocab_test
	if algo_check == "train":
		check_not_in_vocab_train = x not in vocab_train
		if (check_not_in_vocab_train):
			vocab_train[x] = 1
		elif not(check_not_in_vocab_train):
			vocab_train[x] += 1
	elif algo_check == "test":
		check_not_in_vocab_test = x not in vocab_test
		if (check_not_in_vocab_test):
			vocab_test[x] = 1
		elif not(check_not_in_vocab_test):
			vocab_test[x] += 1


def construct_bow(file_path, algo_check):

	n_wrds = 0
	temp_term_freq_dict = {}

	if not os.path.exists(file_path):
		print("******Invalid file name********")
		sys.exit(1)

	with open(file_path, 'r') as f:

		line = f.read()
		
		for term in line.split():

			exclude = set(string.punctuation)
			x = ''.join(ch for ch in term if ch not in exclude)
			x = x.lower()
			
			if x not in set(stopwords):
				
				# x = porter.stem(x)
				n_wrds += 1
				
				if x not in temp_term_freq_dict:

					vocab_check(x, algo_check)
					temp_term_freq_dict[x] = 1
					
				else:
					temp_term_freq_dict[x] +=1
						
	temp_term_freq_dict['no_of_wrds'] = n_wrds
	f.close()
	return temp_term_freq_dict

def construct_tf_idf(tf_dict, no_of_docs, algo_check):

	global vocab_train, vocab_test
	vocab_list = vocab_train.keys()
	tf_idf = np.array([[]], dtype=float)
	
	for doc_id in tf_dict:
		temp=[]
		tf = tf_dict[doc_id]
		for term in vocab_list:
			if term in tf:
				if algo_check == "train":
					n_docs_present = vocab_train[term]
				elif algo_check == "test":
					n_docs_present = vocab_test[term]
				val = (1+ mt.log(tf[term])) * (mt.log((no_of_docs/n_docs_present)))
				temp.append(val)
			else:
				temp.append(0)
		
		if tf_idf.shape == (1,0):
			tf_idf = np.hstack((tf_idf, [temp]))
		else:
			tf_idf = np.vstack((tf_idf, [temp]))

	return tf_idf


def pca(tf_idf):

	# tf_idf_sp = csc_matrix(tf_idf)
	# tf_idf_sp_tr = tf_idf_sp.transpose()
	# print(tf_idf_sp_tr.shape, tf_idf_sp.shape)
	# sigma = tf_idf_sp_tr.multiply(tf_idf_sp)
	# print(sigma.shape)
	# sigma_sp = csc_matrix(sigma)
	# u, s, vt = svds(sigma_sp, k=int(n/2))

	m, n = tf_idf.shape

	for i in range(n):
		temp = tf_idf[:,i]
		#sd = max(temp) - min(temp)
		mean = sum(temp)/len(temp)
		tf_idf[:,i] = (temp - mean)

	sigma = np.dot(np.transpose(tf_idf), tf_idf)
	sigma = (1/m)*sigma
	print("covariance matrix",sigma.shape)
	u, s, vh = np.linalg.svd(sigma)
	k = int(n/2)
	tf_idf_red = np.dot(tf_idf, u[:,0:k])

	print("u matrix",u.shape)
	print("k", k)
	print(s)
	print("variance", sum(s[0:k])/sum(s))

	return tf_idf_red

def construct_class_label(doc_map, n_docs):
	y_label = []
	for i in range(n_docs):
		y_label.append(doc_map[i][0])
	y_label = np.array(y_label)
	return y_label


def tf_idf(train_path, algo_check):

	class_list = out_class_dir_sort(train_path)

	doc_id = 0
	no_of_docs = 0
	doc_map = {}
	tf_dict = OrderedDict({})

	for class_folder in class_list:
		
		class_path = train_path+'/'+str(class_folder)
		class_val  = int(class_folder)
		class_files = in_class_dir_sort(class_path, algo_check)


		for fname in class_files:
			file_nm = class_folder+'_'+fname
			doc_map[doc_id] = (class_val, file_nm)
			file_path = class_path+'/'+str(fname)
			temp_term_freq_dict = construct_bow(file_path, algo_check)
			tf_dict[doc_id] = temp_term_freq_dict
			doc_id += 1
			no_of_docs += 1
			

	tf_idf = construct_tf_idf(tf_dict, no_of_docs, algo_check)

	tf_idf_red = tf_idf #pca(tf_idf)

	#print('reduced tf_idf', tf_idf_red.shape)
	(n_docs, n_feat) = tf_idf_red.shape
	

	y_label = construct_class_label(doc_map, n_docs)

	class_list = sorted(list(map(int,class_list)))

	return (tf_idf_red, y_label, class_list, doc_map)
	
	
	# caluclate the vocab vector for the given query document
	# apply the cosine similarity across all the rows of tf_idf matrix
	# get the first ten doc ids.


def vannila_perceptron(train_x, train_y, epochs, alpha):

	
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
	return np.transpose(W)



def one_vs_all_perceptron(tf_idf_train, train_y, class_list):
	
	class_weights = np.array([[]])

	for label in class_list:
		
		y_label = []
		
		for i in train_y:
			if i == label:
				
				y_label.append(1)
			else:
				y_label.append(-1)
		
		y_label = np.transpose(np.array([y_label]))
		#print('y_label:',y_label)
		
		w = vannila_perceptron(tf_idf_train, y_label, 10, 0.3)
		
		if class_weights.shape == (1,0):
			class_weights = np.hstack((class_weights, w))
		else:
			class_weights = np.vstack((class_weights, w))

	return class_weights
	

def one_vs_all_perceptron_test(class_weights, test_x, test_y):
	
	test_x = test_x.astype(float)
	hyp = np.matmul(test_x, np.transpose(class_weights))
	hyp_class = []
	for temp in hyp:
		temp = list(temp)
		hyp_class.append(temp.index(max(temp)))
	
	mis = 0
	n = len(test_y)
	for i in range(n):
		#print(test_y[i], hyp_class[i])
		if test_y[i] != hyp_class[i]:
			mis += 1
	
	return float((n-mis)/n)



def lsa():

	global vocab_train, vocab_test
	try:
		train_path = sys.argv[1]
		test_path = sys.argv[2]
		query_path = sys.argv[3]
	except IndexError:
		print("Please enter file_paths..eg: >>python lsa.py train_path test_path query_doc_path")
		sys.exit(1)

	print("Constructing tf_idf matrix on 80 percent of training data.......")
	
	(tf_idf_train, y_label, class_list, doc_map_train) = tf_idf(train_path, "train")

	print("Perceptron one vs all training on tf_idf matrix..........")
	class_weights = one_vs_all_perceptron(tf_idf_train, y_label, class_list)
	
	print("Testing on training data (20 percent of data considered as the test dataset).........")
	vocab_test = {}
	(tf_idf_test, y_test, class_list, doc_map_test) = tf_idf(train_path, "test")
	acc = one_vs_all_perceptron_test(class_weights, tf_idf_test, y_test)
	print('******Training accuracy : ',acc,end='**********\n')

	

	print("Testing on test dataset........")
	vocab_test = {}
	(tf_idf_test, y_test, class_list, doc_map_test) = tf_idf(test_path, "test")
	acc = one_vs_all_perceptron_test(class_weights, tf_idf_test, y_test)
	print('******Testing accuracy : ',acc,end='**********\n')


	
	print("Fetching top 10 matched documents....")
	vocab_test = {}
	term_freq_dict = construct_bow(query_path, "test")
	tf_dict = {}
	tf_dict[0] = term_freq_dict
	tf_idf_vec = construct_tf_idf(tf_dict, mt.e, "test")
	
	query_weight = np.dot(tf_idf_train, np.transpose(tf_idf_vec))
	
	r_x = mt.sqrt(sum(tf_idf_vec[0,:] ** 2))
	norm = []
	
	for i in range(len(query_weight)):
		norm.append(mt.sqrt(sum(tf_idf_train[i,:]**2))+r_x)
	norm = np.array(norm)

	for x in range(len(query_weight)):
		query_weight[x] = query_weight[x]/norm[x]

	temp_list = []
	for idx, weight in enumerate(query_weight):
		val = (idx, weight)
		temp_list.append(val)
	final_list = sorted(temp_list, key=lambda x:x[1], reverse=True)
	final_list = final_list[0:10]
	print('Matched Documents: ', end= ' ')
	for idx, w in final_list:
		print(doc_map_train[idx][1], end='  ')
	print('\n')



lsa()




	


	