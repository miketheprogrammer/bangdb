db = require('../../index')('data');

db.open('benchmark_async');

function time(name, start, end) {
  console.log(name, "took:", end - start, "ms");
}

var put_count = 1000;
var get_count = put_count;


console.log("Executing", put_count, "puts");
console.log("Executing", get_count, "gets");
var idx;
function put(count, tme) {
  idx = count.toString();
  db.put(idx,idx, function(err, res) {
    count -= 1;
    if (count >= 0)
      put(count,tme);//return setImmediate(put.bind(null, count, tme));
    else {
      time('PUT', tme, new Date());
      //return get(get_count, new Date());
    }
  });
}

function get(count, tme) {
  idx = count.toString();
  db.get(idx, function (err, res) {
    count -= 1;
    if (count >= 0)
      return setImmediate(get.bind(null, count, tme));
    else time("GET", tme, new Date());
  });
}

put(put_count, new Date());
get(get_count, new Date());