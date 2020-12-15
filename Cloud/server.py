from flask import Flask, render_template, jsonify, make_response
from flask import request
import pandas
import csv
import datetime

app = Flask(__name__)

@app.route('/')
def index():
	return render_template('index.html')

@app.route("/newData")
def newData():
	t = request.args.get("temp")
	d = request.args.get("deskTime")
	print("Temperature: ", t, "C")
	print("Desk Time:", d, "seconds")
	timestamp = datetime.datetime.now() 
	newEntry = [timestamp, t, d]

	global newData
	newData = {'timestamp': timestamp, 'temp': t, 'deskTime': d}

	with open('data.csv', 'a+', newline='') as f:
		writer = csv.writer(f)
		writer.writerow(newEntry)
	return ('', 200)

@app.route("/chartData")
def getChartData():
	df = pandas.read_csv('data.csv')
	recent = df.tail(7).values.tolist()
	res = make_response(jsonify({"recent": recent}), 200)
	return res

@app.route("/updateChart")
def updateChart():
	global newData
	if (newData != None):	
		print(newData.items())
		res = make_response(jsonify({"timestamp": newData['timestamp'], "temp": newData['temp'], "deskTime": newData['deskTime']}), 200)
		newData = None
		return res
	else:
		res = ('', 200)  # Return Empty 200 response if no new data. 
		return res

if __name__ == '__main__':
	newData = None 
	app.run(host='0.0.0.0',debug=True)

