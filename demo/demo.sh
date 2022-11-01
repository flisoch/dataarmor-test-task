#!/bin/sh
node ./Inserts.js $1 $2 $3 $4 $5 &
node ./Updates.js $1 $2 $3 $4 $5 &
node ./Selects.js $1 $2 $3 $4 $5 & 
node ./Deletes.js $1 $2 $3 $4 $5