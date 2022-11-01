const PgDriver = require('./PgDriver');
const Queries = require('./Queries');

const db = new PgDriver();
const queries = new Queries(db);

const args = process.argv;
console.log(args);
const config = {
   "host": args[2], 
   "port": Number(args[3]),
   "user": args[4],
   "password": args[5],
   "database": args[6]
};

db.connect(config, (err, res) => {
   if (err) {
       console.log("connection error.");
       return;
   }
   console.log(res); // connected
});

queries.createDemoTable();
var i = 150;
while (i > 0) {
   queries.deleteQuery();  
   i--;
}
