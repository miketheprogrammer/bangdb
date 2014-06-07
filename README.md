bangdb
======

Officially Revived. Thanks to the folks at IQLECT
Specific API Readme to come.

Currently built against version 1.3 and 1.4 of BangDB.


BangDb bindings for NodeJS. [IQLECTBANGDB](http://iqlect.com)


Roadmap to V0.1.0:

Refactor public accessors to match the BangDB spec more exactly.
 - Like changing from batch to transaction.

Refactor Iterator(*scan*) to allow users to provide a scan_filter

80%+ Test Coverage minimum.

Improved libuv delegation.

Determine a safe way to release the database when a user submits an interrupt to the VM, or the VM crashes. Currently, this results in the database needing to repair itself.

Peer review for memory leaks.

Nice To Have: Create a true batch to minimize overhead from transferring context from JS to C.

Mentions:

The devs of [node-leveldown](https://github.com/rvagg/node-leveldown)Project.
Whom have done alot of work in the node database community, and where I learned how to begin to tackle this project.

