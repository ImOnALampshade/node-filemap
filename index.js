const addon = require('./build/Release/addon');

map = new addon.FileMapping();

async function waitFor(ms) {
  return new Promise(function (resolve, reject) {
    setTimeout(function() {
      return resolve();
    }, ms);
  });
}

function main() {
  return new Promise(async function(resolve, reject) {
    buffer = Buffer.alloc(4) // 4 byte buffer to write to a 4 byte shared storage
    map.openMapping('howard_mem_map', 4)

    for (var i = 0; i < 60; ++i)
    {
      // Wait 1 second between changes
      await waitFor(10);

      // Write the new value
      buffer.writeInt32LE(i);
      map.writeBuffer(buffer, 0, 0, 4);

      console.log('Wrote ' + i);
    }

    map.closeMapping()
    resolve();
  })
}


mainPromise = main()
mainPromise.then(function() {
  console.log('End!')
});
