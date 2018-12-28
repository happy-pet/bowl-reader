function doPost(e) {
  var json_obj = JSON.parse(e.postData.contents);
  var dataValues = json_obj["data"].split(",");//split the data up so we can hold multiple values
  var nextCellValues = [
    json_obj["published_at"],   dataValues[0],   dataValues[1]
  ];
  var deviceID = json_obj["coreid"];
  
  var files = DriveApp.getFilesByName(deviceID);
  if(files.hasNext()){
    //a file exists, good, that should be ours
    var file = files.next();
    if(files.hasNext()){return ContentService.createTextOutput("error, multiple files with deviceID as the name");}//just a sanity check
  } else {
    //a file doesn't exist, so this must be the first time. Let's make a file then
    var file = SpreadsheetApp.create(deviceID, 1, 3);//1 row, 3 collumns
  }
  
  var spreadsheetId = file.getId();
  var spreadsheet = SpreadsheetApp.openById(spreadsheetId);
  
  var sheet = spreadsheet.getSheets()[0];//assume we are using the first sheet
  sheet.appendRow(nextCellValues);
  return ContentService.createTextOutput("success");
}
