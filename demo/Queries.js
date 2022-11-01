
module.exports = class Queries {
    constructor(db) {
        this.db = db;
    }

    createDemoTable() {
        this.db.query("CREATE TABLE IF NOT EXISTS names(name varchar(50) not null);", (err, res) => {
            if (err) {
               console.log("error occurred during query.");
               return;
            }
            console.log(res);
        });
    }
    deleteQuery() { 
        this.db.query("delete from names where name = 'petya';", (err, res) => {
           if (err) {
              console.log("error occurred during query.");
              return;
           }
        //    console.log("delete query!");
        });
    }

    insertQuery() { 
        this.db.query("insert into names(name) values ('ivan');", (err, res) => {
           if (err) {
              console.log("error occurred during query.");
              return;
           }
        //    console.log("insert query!");
        });
    }


    selectQuery() { 
        this.db.query("select * from names limit 1;", (err, res) => {
           if (err) {
              console.log("error occurred during query.");
              return;
           }
        //    console.log("select query!");
        });
    }

    updateQuery() { 
        this.db.query("update names set name ='petya' where name = 'ivan';", (err, res) => {
           if (err) {
              console.log("error occurred during query.");
              return;
           }
        //    console.log("update query!");
        });
    }
}
