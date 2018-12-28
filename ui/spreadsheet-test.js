var googlesheets = require("google-sheets-manager");
var creds = require("./client_secret.json");
 
var ServiceAccount = googlesheets.ServiceAccount;
var GoogleSheet = googlesheets.GoogleSheet;
 
const GOOGLE_SPREADSHEETID = '197Trqsyt9pmfYgvUC5iUPDPfVIoZ2FoxgsGnkm2Pl2c';
const GOOGLE_SHEETID = 0; // update placeholder value
 
var authClass = new ServiceAccount(creds);
var sheetAPI = new GoogleSheet(authClass, GOOGLE_SPREADSHEETID, GOOGLE_SHEETID);
 
var defaultCallback = (err, res) => {
  console.log("lol", err, res)
  if (err) {
    console.log(err);
    throw err;
  }
  console.log(res);
};
 
sheetAPI.getData(defaultCallback);
 
sheetAPI.getData({
    range: {
        startRow: 2,
        startCol: 1,
        endRow: 3,
        endCol: 2,
    },
    majorDimension: "COLUMNS",
}, defaultCallback);
 
sheetAPI.getBatchData(defaultCallback);
 
sheetAPI.getBatchData({
    ranges: [{
        startRow: 2,
        startCol: 1,
        endRow: 3,
        endCol: 2,
    }, {
        startRow:2,
        endCol: 1,
    }],
    majorDimension: "COLUMNS",
}, defaultCallback);
 
sheetAPI.setData([
    ['1', '2'],
    ['3', '4'],
], defaultCallback);
 
sheetAPI.setData([
    ['1', '2'],
    ['3', '4'],
], {
    range: {
        startRow: 2,
        startCol: 1,
        endRow: 3,
        endCol: 2,
    },
    majorDimension: "COLUMNS",
}, defaultCallback);
