db = require('../../index')('data');
db.open('put_benchmark');

var input = [];
for (var i = 0; i < 10; i += 1) {
    input.push({
        a: "hello world",
        b: Date.now(),
        c: "leksakdlaklsdjaksjdaksjdaksjdas",
        d: Math.random(),
        e: "laasldkalsdklaskdlaskdlaskdlasdka",
        f: {
            a: 'asdasdasdasdasd',
            b: 'asdkeoiieiiieeeeere',
            c: Date.now(),
            d: Math.random(),
            e: {
                a: require('fs').readFileSync(__dirname+'/j.js')
            }
        }
    });
}
input = JSON.stringify(input);

function time(name, start, end, suffix) {
    suffix = suffix || 'ms'
    console.log(name, 'took', end - start, suffix);
}

var start = new Date();

for (var i = 0; i < (parseInt(process.argv[2]) || 1000); i += 1) {
    var idx = i.toString()
    db.put(idx, input);
}

time("PUT", start, new Date());
