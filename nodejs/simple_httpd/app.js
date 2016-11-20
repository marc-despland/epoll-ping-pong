var express = require("express");
 var app = express();
 var cntx=0;
 /* serves main page */
 app.get("/", function(req, res) {
    res.sendFile(__dirname + '/index.html')
 });

 var port = 80;
 app.listen(port, function() {
   console.log("Listening on " + port);
 }).on('connection', function(socket) {
  cntx++;
  console.log("open a new conenction : "+cntx);
  socket.setTimeout(30 * 1000); 
  socket.on('close', function(had_error) {
    cntx--;
    console.log("close a conenction : "+cntx);
  });
  // 30 second timeout. Change this as you see fit.
});