// Firebase URL
var firebaseUrl = "";

// Function to add data from Firebase to Google Sheets
function addDataToSheet() {
  // Open the Google Sheets document
  var ss = SpreadsheetApp.openByUrl("");
  var dataLoggerSheet = ss.getSheetByName("Datalogger");
  var minutesSheet = ss.getSheetByName("minutes");

  // Import data from Firebase
  var data = fetchDataFromFirebase();

  // Get the last recorded minute from the Google Sheet
  var lastMinute = minutesSheet.getRange("A1").getValue();

  // If the current minute from Firebase is different, update the Google Sheet
  if (lastMinute != data.ESP.minutes) {
    minutesSheet.getRange("A1").setValue(data.ESP.minutes);
    var ldrDataTop = data.ESP.sensor.topLDR;
    var ldrDataDown = data.ESP.sensor.downLDR;
    var ldrDataRight = data.ESP.sensor.rightLDR;
    var ldrDataLeft = data.ESP.sensor.leftLDR;
    var temperature = data.ESP.sensor.temperature;
    var humidity = data.ESP.sensor.humidity;
    var horizontalServo = data.ESP.servo.horizontal;
    var verticalServo = data.ESP.servo.vertical;

    var dateTime = new Date();
    var time = dateTime.toLocaleTimeString();

    // Get the next empty row in the data logger sheet
    var newRow = dataLoggerSheet.getLastRow() + 1;

    // Populate the data logger sheet with the retrieved data
    dataLoggerSheet.getRange("A" + newRow).setValue(newRow); // ID
    dataLoggerSheet.getRange("B" + newRow).setValue(dateTime); // Date
    dataLoggerSheet.getRange("C" + newRow).setValue(time); // Time
    dataLoggerSheet.getRange("D" + newRow).setValue(ldrDataTop); // Top LDR
    dataLoggerSheet.getRange("E" + newRow).setValue(ldrDataDown); // Down LDR
    dataLoggerSheet.getRange("F" + newRow).setValue(ldrDataRight); // Right LDR
    dataLoggerSheet.getRange("G" + newRow).setValue(ldrDataLeft); // Left LDR
    dataLoggerSheet.getRange("H" + newRow).setValue(temperature); // Temperature
    dataLoggerSheet.getRange("I" + newRow).setValue(humidity); // Humidity
    dataLoggerSheet.getRange("J" + newRow).setValue(horizontalServo); // Horizontal Servo
    dataLoggerSheet.getRange("K" + newRow).setValue(verticalServo); // Vertical Servo
  }
}

// Function to fetch data from Firebase
function fetchDataFromFirebase() {
  try {
    var response = UrlFetchApp.fetch(firebaseUrl);
    var content = response.getContentText();
    var jsonData = JSON.parse(content);

    if (typeof(jsonData) === "undefined") {
      return "Node Not Available";
    } else if (typeof(jsonData) == "object") {
      return jsonData;
    }
  } catch (error) {
    Logger.log(error);
    // return "Error getting data";  
  } 
}
