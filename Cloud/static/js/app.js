var gridOptions = {
	legend: {
		labels: {
			fontColor: 'white',
			fontSize: 18
		}
	},
    scales: {
        xAxes: [{
            gridLines: {
                color: "rgb(255, 255, 255)",
            },
            ticks: {
            	fontColor: "white"
            },
        }],
        yAxes: [{
            gridLines: {
                color: "rgb(255, 255, 255)",
            },
            ticks: {
            	fontColor: "white"
            }
        }]
    }	
}

var timestamps = []
var tempData = []
var timeData = []

window.onload = async function() {
	const res = await axios.get('http://3.88.218.230:5000/chartData')
	const recent = res.data.recent
	for (row of recent){
		t = row[0].split(' ')[1].split('.')[0]
		console.log(t)
		timestamps.push(t)
		tempData.push(row[1])
		timeData.push(row[2])
	}	

	console.log(timestamps)
	console.log(tempData)
	console.log(timeData)	

	var ctx1 = document.getElementById('temp-chart').getContext('2d');
	var ctx2 = document.getElementById('time-chart').getContext('2d');

	var tempChart = new Chart(ctx1, {
	    // The type of chart we want to create
	    type: 'line',

	    // The data for our dataset
	    data: {
	        labels: timestamps,
	        datasets: [{
	            label: 'Temperature',
	            borderColor: 'rgb(255, 99, 132)',
	            backgroundColor: 'rgb(255, 192, 203)',
	            data: tempData
	        }]
	    },
	    // Configuration options go here
	    options: gridOptions
	});

	var timeChart = new Chart(ctx2, {
	    // The type of chart we want to create
	    type: 'line',

	    // The data for our dataset
	    data: {
	        labels: timestamps,
	        datasets: [{
	            label: 'Time Spent at Desk',
	            borderColor: 'rgb(23, 162, 184)',
	            backgroundColor: 'rgb(212, 235, 242)',
	            data: timeData
	        }]
	    },
	    // Configuration options go here
	    options: gridOptions
	});	

	let interval = setInterval(updateChartData, 30000);

	function updateChartData(){
		axios.get('http://3.88.218.230:5000/updateChart').then((res) => {
			if (res.data){
				const timestamp = res.data.timestamp
				const temp = res.data.temp
				const deskTime = res.data.deskTime

				let parsedTimestamp = timestamp.split(' ')[4]

				// Update temperature chart
				tempChart.data.labels.push(parsedTimestamp)
				tempChart.data.labels.splice(0, 1); // remove first label

				tempChart.data.datasets.forEach((dataset) => {
					dataset.data.push(temp)
					dataset.data.splice(0, 1); // remove first data point
				})

				tempChart.update()

				// Update desk time chart 
				timeChart.data.labels.push(parsedTimestamp)
				timeChart.data.labels.splice(0, 1); // remove first label

				timeChart.data.datasets.forEach((dataset) => {
					dataset.data.push(deskTime)
					dataset.data.splice(0, 1); // remove first data point
				})

				timeChart.update()							
			}
		}).catch( (err) => {
			console.log(err)
		})
	}	
}