/*
 *  ToyDB.h
 *  ToyCouch
 *
 *  Created by Jens Alfke on 6/19/10.
 *  Copyright 2010 Jens Alfke. All rights reserved.
 *
 */

#import "ToyRev.h"
@class FMDatabase, ToyRev, ToyRevList, ToyView;


/** Same interpretation as HTTP status codes, esp. 200, 201, 404, 409, 500. */
typedef int ToyDBStatus;


extern NSString* const ToyDBChangeNotification;


typedef struct {
    NSString* startKey;
    NSString* endKey;
    int skip;
    int limit;
    BOOL descending;
    BOOL includeDocs;
    BOOL updateSeq;
} ToyDBQueryOptions;

extern const ToyDBQueryOptions kDefaultToyDBQueryOptions;


/** A ToyCouch database. */
@interface ToyDB : NSObject
{
    @private
    NSString* _path;
    FMDatabase *_fmdb;
    BOOL _open;
    NSInteger _transactionLevel;
    BOOL _transactionFailed;
    NSMutableDictionary* _views;
}    
        
- (id) initWithPath: (NSString*)path;
- (BOOL) open;
#if SQLITE_VERSION_NUMBER >= 3005000
- (BOOL) openWithFlags:(int)flags;
#endif
- (BOOL) close;

+ (ToyDB*) createEmptyDBAtPath: (NSString*)path;

@property (readonly) NSString* path;
@property (readonly) NSString* name;
@property (readonly) BOOL exists;
@property (readonly) int error;

- (void) beginTransaction;
- (void) endTransaction;
@property BOOL transactionFailed;

- (ToyDBStatus) compact;

// DOCUMENTS:

+ (BOOL) isValidDocumentID: (NSString*)str;
- (NSString*) generateDocumentID;

@property (readonly) NSUInteger documentCount;
@property (readonly) SequenceNumber lastSequence;

- (ToyRev*) getDocumentWithID: (NSString*)docID;
- (ToyRev*) getDocumentWithID: (NSString*)docID revisionID: (NSString*)revID;
- (ToyDBStatus) loadRevisionBody: (ToyRev*)rev;

/** Returns an array of ToyRevs in reverse chronological order,
    starting with the given revision. */
- (NSArray*) getRevisionHistory: (ToyRev*)rev;
- (ToyRevList*) getAllRevisionsOfDocumentID: (NSString*)docID;

- (ToyRev*) putRevision: (ToyRev*)revision
         prevRevisionID: (NSString*)revID
                 status: (ToyDBStatus*)outStatus;
- (ToyDBStatus) forceInsert: (ToyRev*)rev
            revisionHistory: (NSArray*)history;

- (NSArray*) changesSinceSequence: (int)lastSequence
                          options: (const ToyDBQueryOptions*)options;

// VIEWS & QUERIES:

- (NSDictionary*) getAllDocs: (const ToyDBQueryOptions*)options;

- (ToyView*) viewNamed: (NSString*)name;
@property (readonly) NSArray* allViews;

// FOR REPLICATION:

- (BOOL) findMissingRevisions: (ToyRevList*)toyRevs;

@end