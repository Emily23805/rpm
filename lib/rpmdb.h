#ifndef H_RPMDB
#define H_RPMDB

/** \ingroup rpmdb dbi
 * \file lib/rpmdb.h
 * Access RPM indices using Berkeley DB interface(s).
 */

#include <rpm/rpmtypes.h>
#include <rpm/rpmsw.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Tag value pattern match mode.
 */
typedef enum rpmMireMode_e {
    RPMMIRE_DEFAULT	= 0,	/*!< regex with \., .* and ^...$ added */
    RPMMIRE_STRCMP	= 1,	/*!< strings  using strcmp(3) */
    RPMMIRE_REGEX	= 2,	/*!< regex(7) patterns through regcomp(3) */
    RPMMIRE_GLOB	= 3	/*!< glob(7) patterns through fnmatch(3) */
} rpmMireMode;

typedef enum rpmdbOpX_e {
    RPMDB_OP_DBGET              = 1,
    RPMDB_OP_DBPUT              = 2,
    RPMDB_OP_DBDEL              = 3,
    RPMDB_OP_MAX		= 4
} rpmdbOpX;

/** \ingroup rpmdb
 * Retrieve operation timestamp from rpm database.
 * @param db            rpm database
 * @param opx           operation timestamp index
 * @return              pointer to operation timestamp.
 */
rpmop rpmdbOp(rpmdb db, rpmdbOpX opx);

/** \ingroup rpmdb
 * Reference a database instance.
 * @param db		rpm database
 * @return		new rpm database reference
 */
rpmdb rpmdbLink(rpmdb db);

/** \ingroup rpmdb
 * Open rpm database.
 * @param prefix	path to top of install tree
 * @retval dbp		address of rpm database
 * @param mode		open(2) flags:  O_RDWR or O_RDONLY (O_CREAT also)
 * @param perms		database permissions
 * @return		0 on success
 */
int rpmdbOpen (const char * prefix, rpmdb * dbp,
		int mode, int perms);

/** \ingroup rpmdb
 * Initialize database.
 * @param prefix	path to top of install tree
 * @param perms		database permissions
 * @return		0 on success
 */
int rpmdbInit(const char * prefix, int perms);

/** \ingroup rpmdb
 * Verify database components.
 * @param prefix	path to top of install tree
 * @return		0 on success
 */
int rpmdbVerify(const char * prefix);

/** \ingroup rpmdb
 * Close all database indices and free rpmdb.
 * @param db		rpm database
 * @return		0 on success
 */
int rpmdbClose (rpmdb db);

/** \ingroup rpmdb
 * Sync all database indices.
 * @param db		rpm database
 * @return		0 on success
 */
int rpmdbSync (rpmdb db);

/** \ingroup rpmdb
 * Open all database indices.
 * @param db		rpm database
 * @return		0 on success
 */
int rpmdbOpenAll (rpmdb db);

/** \ingroup rpmdb
 * Return number of instances of package in rpm database.
 * @param db		rpm database
 * @param name		rpm package name
 * @return		number of instances
 */
int rpmdbCountPackages(rpmdb db, const char * name);

/** \ingroup rpmdb
 * Return header join key for current position of rpm database iterator.
 * @param mi		rpm database iterator
 * @return		current header join key
 */
unsigned int rpmdbGetIteratorOffset(rpmdbMatchIterator mi);

/** \ingroup rpmdb
 * Return number of elements in rpm database iterator.
 * @param mi		rpm database iterator
 * @return		number of elements
 */
int rpmdbGetIteratorCount(rpmdbMatchIterator mi);

/** \ingroup rpmdb
 */
unsigned int rpmdbGetIteratorFileNum(rpmdbMatchIterator mi);

/** \ingroup rpmdb
 * Append items to set of package instances to iterate.
 * @param mi		rpm database iterator
 * @param hdrNums	array of package instances
 * @param nHdrNums	number of elements in array
 * @return		0 on success, 1 on failure (bad args)
 */
int rpmdbAppendIterator(rpmdbMatchIterator mi,
		const int * hdrNums, int nHdrNums);

/** \ingroup rpmdb
 * Add pattern to iterator selector.
 * @param mi		rpm database iterator
 * @param tag		rpm tag
 * @param mode		type of pattern match
 * @param pattern	pattern to match
 * @return		0 on success
 */
int rpmdbSetIteratorRE(rpmdbMatchIterator mi, rpmTag tag,
		rpmMireMode mode, const char * pattern);

/** \ingroup rpmdb
 * Prepare iterator for lazy writes.
 * @note Must be called before rpmdbNextIterator() with CDB model database.
 * @param mi		rpm database iterator
 * @param rewrite	new value of rewrite
 * @return		previous value
 */
int rpmdbSetIteratorRewrite(rpmdbMatchIterator mi, int rewrite);

/** \ingroup rpmdb
 * Modify iterator to mark header for lazy write on release.
 * @param mi		rpm database iterator
 * @param modified	new value of modified
 * @return		previous value
 */
int rpmdbSetIteratorModified(rpmdbMatchIterator mi, int modified);

/** \ingroup rpmdb
 * Modify iterator to verify retrieved header blobs.
 * @param mi		rpm database iterator
 * @param ts		transaction set
 * @param (*hdrchk)	headerCheck() vector
 * @return		0 always
 */
int rpmdbSetHdrChk(rpmdbMatchIterator mi, rpmts ts,
	rpmRC (*hdrchk) (rpmts ts, const void * uh, size_t uc, char ** msg));

/** \ingroup rpmdb
 * Return database iterator.
 * @param db		rpm database
 * @param rpmtag	rpm tag
 * @param keyp		key data (NULL for sequential access)
 * @param keylen	key data length (0 will use strlen(keyp))
 * @return		NULL on failure
 */
rpmdbMatchIterator rpmdbInitIterator(rpmdb db, rpmTag rpmtag,
			const void * keyp, size_t keylen);

/** \ingroup rpmdb
 * Return next package header from iteration.
 * @param mi		rpm database iterator
 * @return		NULL on end of iteration.
 */
Header rpmdbNextIterator(rpmdbMatchIterator mi);

/** \ingroup rpmdb
 * Check for and exit on termination signals.
 */
int rpmdbCheckSignals(void);

/** \ingroup rpmdb
 * Check rpmdb signal handler for trapped signal and/or requested exit,
 * clean up any open iterators and databases on termination condition.
 * On non-zero exit any open references to rpmdb are invalid and cannot
 * be accessed anymore, calling process should terminate immediately.
 * @param terminate	0 to only check for signals, 1 to terminate anyway
 * @return 		0 to continue, 1 if termination cleanup was done.
 */
int rpmdbCheckTerminate(int terminate);

/** \ingroup rpmdb
 * Destroy rpm database iterator.
 * @param mi		rpm database iterator
 * @return		NULL always
 */
rpmdbMatchIterator rpmdbFreeIterator(rpmdbMatchIterator mi);

/** \ingroup rpmdb
 * Rebuild database indices from package headers.
 * @param prefix	path to top of install tree
 * @param ts		transaction set (or NULL)
 * @param (*hdrchk)	headerCheck() vector (or NULL)
 * @return		0 on success
 */
int rpmdbRebuild(const char * prefix, rpmts ts,
		rpmRC (*hdrchk) (rpmts ts, const void *uh, size_t uc, char ** msg));




/** \ingroup rpmdb
 * Get a key iterator for an index
 * @param db		rpm database
 * @param rpmtag	the index to iterate over
 * @return		the key iterator
 */
rpmdbKeyIterator rpmdbKeyIteratorInit(rpmdb db, rpmTag rpmtag);

/** \ingroup rpmdb
 * Get the next key - must be called before getting the first key
 * @param ki		key iterator
 * @return 		0 on success; != 0 on error or end of index
 */
int rpmdbKeyIteratorNext(rpmdbKeyIterator ki);

/** \ingroup rpmdb
 * Get current key
 * @param ki            key iterator
 * @return		pointer to key content. Keys are not zero terminated!
 */
const void * rpmdbKeyIteratorKey(rpmdbKeyIterator ki);

/** \ingroup rpmdb
 * Get length of key
 * @param ki            key iterator
 * @return 		length of key
 */
size_t rpmdbKeyIteratorKeySize(rpmdbKeyIterator ki);

/** \ingroup rpmdb
 * Free key iterator
 * @param ki            key iterator
 * return 		NULL
 */
rpmdbKeyIterator rpmdbKeyIteratorFree(rpmdbKeyIterator ki);


#ifdef __cplusplus
}
#endif

#endif	/* H_RPMDB */
