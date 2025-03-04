Releasing a version
===================

Preliminaries
-------------

- Make sure that ``RELEASE_NOTES.md`` and ``ANNOUNCE.md`` are up to
  date with the latest news in the release.

- Check that *VERSION* symbols in include/blosc2.h contains the correct info.

- Commit the changes with::

    $ git commit -a -m "Getting ready for release X.Y.Z"
    $ git push


Testing
-------

Create a new build/ directory, change into it and issue::

  $ cmake ..
  $ cmake --build .
  $ ctest


Forward compatibility testing
-----------------------------

First, go to the compat/ directory and generate a file with the current
version::

  $ cd ../compat
  $ export LD_LIBRARY_PATH=../build/blosc
  $ gcc -o filegen filegen.c -L$LD_LIBRARY_PATH -lblosc2 -I../blosc
  $ ./filegen compress lz4 blosc-lz4-1.y.z.cdata

In order to make sure that we are not breaking forward compatibility,
link and run the `compat/filegen` utility against different versions of
the Blosc library (suggestion: 1.3.0, 1.7.0, 1.11.1, 1.14.1, 2.0.0).

You can compile the utility with different blosc shared libraries with::

  $ export LD_LIBRARY_PATH=shared_blosc_library_path
  $ gcc -o filegen filegen.c -L$LD_LIBRARY_PATH -lblosc -Iblosc.h_include_path

Then, test the file created with the new version with::

  $ ./filegen decompress blosc-lz4-1.y.z.cdata

Repeat this for every codec shipped with Blosc (blosclz, lz4, lz4hc, zlib and
zstd).

Tagging
-------

- Create a tag ``X.Y.Z`` from ``master``.  Use the next message::

    $ git tag -a vX.Y.Z -m "Tagging version X.Y.Z"

- Push the tag to the github repo::

    $ git push --tags

- Create a new release visiting https://github.com/Blosc/c-blosc2/releases/new
  and add the release notes copying them from `RELEASE_NOTES.md` document.


Check documentation
-------------------

Go the the `blosc2 rtfd <https://c-blosc2.readthedocs.io/>`_ site and check that
it contains the updated docs.


Announcing
----------

- Send an announcement to the blosc and comp.compression mailing lists.
  Use the ``ANNOUNCE.md`` file as skeleton (likely as the definitive version).

- Tweet about it from the @Blosc2 account.


Post-release actions
--------------------

- Edit *VERSION* symbols in blosc/blosc.h in master to increment the
  version to the next minor one (i.e. X.Y.Z --> X.Y.(Z+1).dev).

- Create new headers for adding new features in ``RELEASE_NOTES.md``
  and empty the release-specific information in ``ANNOUNCE.md`` and
  add this place-holder instead:

  #XXX version-specific blurb XXX#

- Commit the changes::

  $ git commit -a -m"Post X.Y.Z release actions done"
  $ git push

That's all folks!


.. Local Variables:
.. mode: rst
.. coding: utf-8
.. fill-column: 70
.. End:
