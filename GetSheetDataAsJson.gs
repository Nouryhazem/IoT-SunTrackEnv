// Function to handle HTTP GET requests
function doGet(request) {
  // Open Google Sheet using its URL
  var ss = "";

  // Get the sheet named "Datalogger"
  var sheet = ss.getSheetByName("Datalogger");

  // Get all values in the active sheet
  var values = sheet.getDataRange().getValues();
  var data = [];

  // Iterate over values in descending order
  for (var i = values.length - 1; i >= 0; i--) {
    // Get each row
    var row = values[i];

    // Create an object to store row data
    var rowData = {};

    // Assign values to the object properties
    rowData['index'] = row[0]; // Assuming index is in the first column
    rowData['date'] = row[1]; // Assuming date is in the second column
    rowData['value'] = row[3]; // Assuming value is in the fourth column

    // Push the row data object to the data array
    data.push(rowData);
  }

  // Log the data for debugging purposes
  Logger.log(data);

  // Return the data as a JSON response
  return ContentService
    .createTextOutput(JSON.stringify(data))
    .setMimeType(ContentService.MimeType.JSON);
}
