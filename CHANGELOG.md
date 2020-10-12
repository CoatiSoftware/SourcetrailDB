# Changelog

## v4.db25.p0

**2020-10-12**

* Interface Change: fix "Ambiuous" typo in interface instead of "Ambiguous" (issue #8)
* Added Java bindings for API
* Added Perl bindings for API


## v3.db25.p0

**2019-11-04**

* Interface Change: removed unused node and edge types


## v2.db24.p0

**2019-05-20**

* Interface Extension: added methods to record edges as ambiguous with new database version


## v2.db23.p2

**2019-04-02**

* Interface Extension: `getVersionString()` is not available via API


## v2.db23.p1

**2019-03-26**

* Interface Extension: added method to store a qualifier location to the database


## v2.db23.p0

**2019-02-26**

* Interface Change: renamed `recordCommentLocation()` to `recordAtomicSourceRange()` to be more generic
* Fix: build issue if tags of this repo are not checked out (issue #1)


## v1.db23.p0

**2018-12-19**

* First official release of the SourcetrailDB project.
