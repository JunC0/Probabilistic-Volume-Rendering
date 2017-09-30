import numpy as np
import sys
import os
import time
import pickle
from sklearn.ensemble import RandomForestClassifier
from sklearn.cross_validation import KFold
nb_input_size   = int(sys.argv[1]) #784
nb_classes 		= int(sys.argv[2])
nb_folds		= 4
nb_iter			= 20
RANDOM_STATE	= 633########################################################

if __name__ == '__main__':
	start_time=time.time()
	X = np.fromfile(sys.argv[3], dtype=np.float32)
	y = np.fromfile(sys.argv[4], dtype=np.float32)
	X = np.reshape(X, (X.shape[0]/nb_input_size, nb_input_size))
	X = X.astype(np.float32)

	rfc=RandomForestClassifier(n_estimators=30,
                               max_features="sqrt")
							   
	
	rfc.fit(X,y)

	print ('***Train Time: %s seconds'% (time.time()-start_time))

	start_time=time.time()

	

	X_test = np.fromfile(sys.argv[5], dtype=np.uint8)

	"""
	X_test = np.reshape(X_test, (X_test.shape[0]/nb_input_size, nb_input_size))
	
	X_test = X_test.astype(np.int8)
	"""
	
	rfc.predict_cuda(X_test)

	print ('***Predict Time: %s seconds'% (time.time()-start_time))


	
    