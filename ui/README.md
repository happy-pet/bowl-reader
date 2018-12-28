# Creating a service account:

1. Create a project in the google API console (we called it bowl-reader).

https://console.developers.google.com/projectcreate?previousPage=%2Fiam-admin%2Fserviceaccounts%3Fproject%3Dbowl-reader&organizationId=0

2. Create a service account for said project:

https://console.developers.google.com/iam-admin/serviceaccounts?project=bowl-reader

Download the key as a JSON file. These are our credentials.

3. Share the sheet with the service account email address (client_email in the client secret JSON); view rights needed.

4. Request some data using the javascript lib; first request will fail and the error message will have a link to turn on API access probably.
