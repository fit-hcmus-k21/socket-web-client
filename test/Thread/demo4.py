from urlparse import urlparse
from threading import Thread
import httplib, sys
from Queue import Queue

concurrent = 200

def doWork(): 
    while True:
        url = q.get() # get a url from the queue
        status, url = getStatus(url) # get the status of the url
        doSomethingWithResult(status, url) # do something with the result
        q.task_done() # the work queue

def getStatus(ourl): # get the status of the url
    try:
        url = urlparse(ourl) # parse the url
        conn = httplib.HTTPConnection(url.netloc)  # create a connection  
        conn.request("HEAD", url.path) # send a HEAD request
        res = conn.getresponse() # get the response
        return res.status, ourl # return the status and the url
    except:
        return "error", ourl # if there is an error return error and the url

def doSomethingWithResult(status, url): # do something with the result
    print status, url # print the status and the url

q = Queue(concurrent * 2) # create a queue
for i in range(concurrent): # create a thread pool
    t = Thread(target=doWork) # create a thread
    t.daemon = True # set the thread as a daemon
    t.start() # start the thread
try:
    for url in open('urllist.txt'): # open the url list
        q.put(url.strip()) # put the url in the queue
    q.join()    # block until all tasks are done
except KeyboardInterrupt: # if there is a keyboard interrupt
    sys.exit(1) # exit the program