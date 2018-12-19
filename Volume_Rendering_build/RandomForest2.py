import numpy as np
import sys
import os
import time
from sklearn.ensemble import RandomForestClassifier
nb_input_size   = int(sys.argv[1]) #784
nb_classes 		= int(sys.argv[2])

########################################################
if __name__ == '__main__':
	start_time=time.time()
	X = np.fromfile(sys.argv[3], dtype=np.float32)
	y = np.fromfile(sys.argv[4], dtype=np.float32)
	X = np.reshape(X, (int(X.shape[0]/nb_input_size), nb_input_size))
	X = X.astype(np.float32)

	rfc=RandomForestClassifier(n_estimators=50, oob_score=True,
                               max_features="sqrt")
							   
	rfc.fit(X,y)
	print('***oob_score: %s'% rfc.oob_score_)
	
	
	print ('***Train Time: %s seconds'% (time.time()-start_time))
	start_time=time.time()
	
	X_test = np.fromfile(sys.argv[5], dtype=np.float32)
	print(X_test.shape[0])
	print(nb_input_size)
	print(X_test.shape[0]/nb_input_size)
	X_test = np.reshape(X_test, (int(X_test.shape[0]/nb_input_size), nb_input_size))
	X_test = X_test.astype(np.float32)
	
	y_pred=rfc.predict_proba(X_test)
	
	
	y_pred = y_pred.astype(np.float32)
	y_pred.tofile('probability')
	
	print ('***Predict Time: %s seconds'% (time.time()-start_time))