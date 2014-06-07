db = require('../../index')('data');
var microtime = require('microtime');
db.open('batch_put_benchmark');
var assert = require('assert');


function time(name, start, end, suffix) {
    suffix = suffix || 'ms'
  console.log(name, 'took', end - start, suffix);
}
function test() {
    batch_count = 1000000;
    var tme;
    function cb(err, res) {
        time('Batch:', tme, new Date());
    }

    var _batch = [];
    var idx;

    for (var i = 0; i < batch_count; i += 1) {
        var idx = i.toString();
        _batch.push({type:'put', key:idx, value:idx});
    }
    var ptme = new Date();
    for (var i = 0; i < batch_count; i += 1) {
        var idx = i.toString();
        //var txe = microtime.now();
        db.put(idx, idx);
        //time("PUT OP", txe, microtime.now(), 'us')
    }
    time("PUT", ptme, new Date());
    var tme = new Date();
    //db.batch(_batch, cb);
}


function doTest(count) {
    count -= 1;
    if (count > 0) {
        //setTimeout(test, 100);
        test();
        //setTimeout(doTest.bind(null, count), 140);
    } 
        var lastkey;
        for (var i = 0; i < batch_count; i += 1) {
            var idx = i.toString();
            //db.put(idx, idx);

            var newkey = db.get(idx);
            console.log(newkey);
            if (lastkey !== undefined) {
                console.log(newkey, parseInt(lastkey) < parseInt(newkey))
                assert.ok(parseInt(lastkey) === ( parseInt(newkey) - 1) );
            }

            lastkey = newkey;
        }
    
}

doTest(2);
