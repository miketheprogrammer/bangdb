db = require('../../index')('data');
db.open('put_benchmark');


function time(name, start, end, suffix) {
    suffix = suffix || 'ms'
    console.log(name, 'took', end - start, suffix);
}

var start = new Date();

for (var i = 0; i < (parseInt(process.argv[2]) || 1000); i += 1) {
    var idx = i.toString()
    db.get(idx);
}

time("GET", start, new Date());