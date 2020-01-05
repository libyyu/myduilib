#define FLIB_DLL
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include "FMemory.h"
//#define _F_USE_MEMTRACK
//#undef new    // IMPORTANT!

namespace
{
	/* ------------------------------------------------------------ */
	/* --------------------- class BlockHeader -------------------- */
	/* ------------------------------------------------------------ */
	class FBlockHeader
	{
	public:
		friend class BlockHeaderStatic;
		class BlockHeaderStatic
		{
		protected:
			BlockHeaderStatic() :ourFirstNode(NULL) {}
		public:
			FBlockHeader * ourFirstNode;
			inline static BlockHeaderStatic& Get()
			{
				static BlockHeaderStatic instance;
				return instance;
			}

			inline void AddNode(FBlockHeader *node)
			{
				assert(node != NULL);
				assert(node->myPrevNode == NULL);
				assert(node->myNextNode == NULL);

				// If we have at least one node in the list ...        
				if (ourFirstNode != NULL)
				{
					// ... make the new node the first node's predecessor.
					assert(ourFirstNode->myPrevNode == NULL);
					ourFirstNode->myPrevNode = node;
				}

				// Make the first node the new node's succesor.
				node->myNextNode = ourFirstNode;

				// Make the new node the first node.
				ourFirstNode = node;
			}
			inline void RemoveNode(FBlockHeader *node)
			{
				assert(node != NULL);
				assert(ourFirstNode != NULL);

				// If target node is the first node in the list...
				if (ourFirstNode == node)
				{
					// ... make the target node's successor the first node.
					assert(ourFirstNode->myPrevNode == NULL);
					ourFirstNode = node->myNextNode;
				}

				// Link target node's predecessor, if any, to its successor.
				if (node->myPrevNode != NULL)
				{
					node->myPrevNode->myNextNode = node->myNextNode;
				}

				// Link target node's successor, if any, to its predecessor.
				if (node->myNextNode != NULL)
				{
					node->myNextNode->myPrevNode = node->myPrevNode;
				}

				// Clear target node's previous and next pointers.
				node->myPrevNode = NULL;
				node->myNextNode = NULL;
			}
			inline size_t CountBlocks()
			{
				size_t count = 0;
				FBlockHeader *currNode = ourFirstNode;
				while (currNode != NULL)
				{
					count++;
					currNode = currNode->myNextNode;
				}
				return count;
			}
			inline void GetBlocks(FBlockHeader **blockHeaderPP)
			{
				FBlockHeader *currNode = ourFirstNode;
				while (currNode != NULL)
				{
					*blockHeaderPP = currNode;
					blockHeaderPP++;
					currNode = currNode->myNextNode;
				}
			}
		};
	private:    // member variables
		FBlockHeader * myPrevNode;
		FBlockHeader *myNextNode;
		size_t myRequestedSize;
		char const *myFilename;
		char const *myFuncname;
		int myLineNum;
		char const *myTypeName;
	public:     // members
		FBlockHeader(size_t requestedSize)
		{
			myPrevNode = NULL;
			myNextNode = NULL;
			myRequestedSize = requestedSize;
			myFilename = "[unknown]";
			myFuncname = "[unknown]";
			myLineNum = 0;
			myTypeName = "[unknown]";
		}
		~FBlockHeader() {}

		inline size_t GetRequestedSize() const { return myRequestedSize; }
		inline char const *GetFilename() const { return myFilename; }
		inline char const *GetFuncname() const { return myFuncname; }
		inline int GetLineNum() const { return myLineNum; }
		inline char const *GetTypeName() const { return myTypeName; }
		inline void Stamp(char const *filename, char const *funcname, int lineNum, char const *typeName)
		{
			myFilename = filename ? filename : "[unknown]";
			myFuncname = funcname ? funcname : "[unknown]";
			myLineNum = lineNum;
			myTypeName = typeName ? typeName : "[unknown]";
		}
		static bool TypeGreaterThan(FBlockHeader *header1, FBlockHeader *header2)
		{
			return (strcmp(header1->myTypeName, header2->myTypeName) > 0);
		}

		size_t size;
		size_t magic;
	};

	/* ------------------------------------------------------------ */
	/* ---------------------- class FSignature --------------------- */
	/* ------------------------------------------------------------ */
	class FSignature
	{
	private:    // constants
		static const unsigned int SIGNATURE1 = 0xCAFEBABE;
		static const unsigned int SIGNATURE2 = 0xFACEFACE;

	private:    // member variables
		unsigned int mySignature1;
		unsigned int mySignature2;

	public:        // construction/destruction
		FSignature() : mySignature1(SIGNATURE1), mySignature2(SIGNATURE2) {};
		~FSignature() { mySignature1 = 0; mySignature2 = 0; }

	public:        // static member functions
		static bool IsValidSignature(const FSignature *pProspectiveSignature)
		{
			try
			{
				if (pProspectiveSignature->mySignature1 != SIGNATURE1) return false;
				if (pProspectiveSignature->mySignature2 != SIGNATURE2) return false;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}
	};

	/* ------------------------------------------------------------ */
	/* -------------------- address conversion -------------------- */
	/* ------------------------------------------------------------ */

	/* We divide the memory blocks we allocate into two "chunks", the
	* "prolog chunk" where we store information about the allocation,
	* and the "user chunk" which we return to the caller to use.
	*/

	/* ---------------------------------------- alignment */
	static const size_t FALIGNMENT = 4;
	static const size_t PROLOG_MAGIC = 0x11223344;

	/* If "value" (a memory size or offset) falls on an alignment boundary,
	* then just return it.  Otherwise return the smallest number larger
	* than "value" that falls on an alignment boundary.
	*/
#define FPAD_TO_ALIGNMENT_BOUNDARY(value) \
        ((value) + ((FALIGNMENT - ((value) % FALIGNMENT)) % FALIGNMENT))

	/* ---------------------------------------- chunk structs */
	/* We declare incomplete structures for each chunk, just to
	* provide type safety.
	*/

	struct FPrologChunk;
	struct FUserChunk;

	/* ---------------------------------------- chunk sizes and offsets */
	static const size_t FSIZE_BlockHeader = FPAD_TO_ALIGNMENT_BOUNDARY(sizeof(FBlockHeader));
	static const size_t FSIZE_Signature = FPAD_TO_ALIGNMENT_BOUNDARY(sizeof(FSignature));

	static const size_t FOFFSET_BlockHeader = 0;
	static const size_t FOFFSET_Signature = FOFFSET_BlockHeader + FSIZE_BlockHeader;
	static const size_t FOFFSET_UserChunk = FOFFSET_Signature + FSIZE_Signature;

	static const size_t FSIZE_PrologChunk = FOFFSET_UserChunk;

	/* ---------------------------------------- GetUserAddress */
	static FUserChunk *FGetUserAddress(FPrologChunk *pProlog)
	{
		char *pchProlog = reinterpret_cast<char *>(pProlog);
		char *pchUser = pchProlog + FOFFSET_UserChunk;
		FUserChunk *pUser = reinterpret_cast<FUserChunk *>(pchUser);
		return pUser;
	}

	/* ---------------------------------------- GetPrologAddress */
	static FPrologChunk *FGetPrologAddress(FUserChunk *pUser)
	{
		char *pchUser = reinterpret_cast<char *>(pUser);
		char *pchProlog = pchUser - FOFFSET_UserChunk;
		FPrologChunk *pProlog = reinterpret_cast<FPrologChunk *>(pchProlog);
		return pProlog;
	}

	/* ---------------------------------------- GetHeaderAddress */
	static FBlockHeader *FGetHeaderAddress(FPrologChunk *pProlog)
	{
		char *pchProlog = reinterpret_cast<char *>(pProlog);
		char *pchHeader = pchProlog + FOFFSET_BlockHeader;
		FBlockHeader *pHeader = reinterpret_cast<FBlockHeader *>(pchHeader);
		return pHeader;
	}

	/* ---------------------------------------- GetSignatureAddress */
	static FSignature *FGetSignatureAddress(FPrologChunk *pProlog)
	{
		char *pchProlog = reinterpret_cast<char *>(pProlog);
		char *pchSignature = pchProlog + FOFFSET_Signature;
		FSignature *pSignature = reinterpret_cast<FSignature *>(pchSignature);
		return pSignature;
	}

	/* ---------------------------------------- FAllocStamp */
	static void FAllocStamp(void *p, const _FStd(FMemory::FMemStamp) &stamp, char const *typeName)
	{
		// Get the header and signature address for this pointer.
		FUserChunk *pUser = reinterpret_cast<FUserChunk *>(p);
		FPrologChunk *pProlog = FGetPrologAddress(pUser);
		FBlockHeader *pHeader = FGetHeaderAddress(pProlog);
		FSignature *pSignature = FGetSignatureAddress(pProlog);

		// If the signature is not valid, then return immediately.
		if (!FSignature::IsValidSignature(pSignature)) return;

		// "Stamp" the information onto the header.
		pHeader->Stamp(stamp.filename, stamp.func, stamp.lineNum, typeName);
	}

    /* ------------------------------------------------------------ */
    /* -------------- memory allocation and stamping -------------- */
    /* ------------------------------------------------------------ */
    static void *FMalloc(size_t size, const char* file, const char* func, int line)
    {
        void* pResult = NULL;
    #ifdef TRACK_MEMORY_DEBUG
        // Allocate the memory, including space for the prolog.
        FPrologChunk *pProlog = (FPrologChunk *)malloc(FSIZE_PrologChunk + size);
        
        // If the allocation failed, then return NULL.
        if (pProlog == NULL) return NULL;
        
        // Use placement new to construct the block header in place.
        FBlockHeader *pBlockHeader = new (pProlog) FBlockHeader(size);
        pBlockHeader->size = size;
        pBlockHeader->magic = PROLOG_MAGIC;
        
        // Link the block header into the list of extant block headers.
        FBlockHeader::BlockHeaderStatic::Get().AddNode(pBlockHeader);
        
        // Use placement new to construct the signature in place.
        FSignature *pSignature = new (FGetSignatureAddress(pProlog)) FSignature;
        
        // Get the offset to the user chunk and return it.
        FUserChunk *pUser = FGetUserAddress(pProlog);
        
		_FStd(FMemory::FMemStamp) stamp(file, func, line);
        FAllocStamp(pUser, stamp, NULL);

        pResult = pUser;
    #else
        pResult = malloc(size);
    #endif

        return pResult;
    }

    static void FFree(void *p)
    {
        // It's perfectly valid for "p" to be null; return if it is.
        if (p == NULL) return;
    #ifdef TRACK_MEMORY_DEBUG
        // Get the prolog address for this memory block.
        FUserChunk *pUser = reinterpret_cast<FUserChunk *>(p);    
        FPrologChunk *pProlog = FGetPrologAddress(pUser);
        
        // Check the signature, and if it's invalid, return immediately.
        FSignature *pSignature = FGetSignatureAddress(pProlog);
        if (!FSignature::IsValidSignature(pSignature)) return;
        
        // Destroy the signature.
        pSignature->~FSignature();
        pSignature = NULL;

        // Unlink the block header from the list and destroy it.
        FBlockHeader *pBlockHeader = FGetHeaderAddress(pProlog);
        assert(pBlockHeader->magic == PROLOG_MAGIC);
        FBlockHeader::BlockHeaderStatic::Get().RemoveNode(pBlockHeader);
        pBlockHeader->~FBlockHeader();
        pBlockHeader = NULL;

        // Free the memory block.    
        free(pProlog);
    #else
        free(p);
    #endif
    }

    static void *FRealloc(void* p, size_t size, const char* file, const char* func, int line)
    {
    #ifdef TRACK_MEMORY_DEBUG
        void *newptr = NULL;
        if(p)
        {
            FUserChunk *pUser = reinterpret_cast<FUserChunk *>(p);    
            FPrologChunk *pProlog = FGetPrologAddress(pUser);
            FBlockHeader *pBlockHeader = FGetHeaderAddress(pProlog);
            assert(pBlockHeader->magic == PROLOG_MAGIC);
            size_t oldsize = pBlockHeader->size;
            if (size <= oldsize) 
            {
                newptr = p;
            } 
            else
            {
                newptr = FMalloc(size, file, func, line);
                if(newptr)
                {
                    memcpy(newptr, p, size < oldsize ? size : oldsize);
                    FFree(p);
                }
            }  
        }
        else
        {
            if(size >0)
            {
                newptr = FMalloc(size, file, func, line);
            }
        }
        if(newptr == p)
        {
            return newptr;
        }

        return newptr;
    #else
        return realloc(p, size);
    #endif
    }

    static void *FCalloc(size_t nmemb, size_t size, const char* file, const char* func, int line)
    {
    #ifdef TRACK_MEMORY_DEBUG
        size_t nsize = nmemb * size;
        void* ptr = FMalloc(nsize, file, func, line);
        if (ptr != NULL) 
        {
            memset(ptr, 0x00, nsize);
        }
        return ptr;
    #else
        return calloc(nmemb, size);
    #endif
    }

#ifdef _F_USE_MEMTRACK
    /* ---------------------------------------- TrackDumpBlocks */
    inline void FTrackDumpBlocks()
    {
        // Get an array of pointers to all extant blocks.
        size_t numBlocks = FBlockHeader::BlockHeaderStatic::Get().CountBlocks();
        FBlockHeader **ppBlockHeader = (FBlockHeader **)calloc(numBlocks, sizeof(*ppBlockHeader));
        FBlockHeader::BlockHeaderStatic::Get().GetBlocks(ppBlockHeader);
        FILE* fp = fopen("MemoryBlocks.txt", "w");
        if(!fp) return;
        // Dump information about the memory blocks.
        fprintf(fp, "\n");
        fprintf(fp, "-------------------------------------------------------------------\n");
        fprintf(fp, "                      Current Memory Blocks                        \n");
        fprintf(fp, "-------------------------------------------------------------------\n");
        fprintf(fp, "\n");
        int index = 0;
        for (size_t i = 0; i < numBlocks; i++)
        {
            FBlockHeader *pBlockHeader = ppBlockHeader[i];
            char const *typeName = pBlockHeader->GetTypeName();
            size_t size = pBlockHeader->GetRequestedSize();
            char const *fileName = pBlockHeader->GetFilename();
            char const *funcName = pBlockHeader->GetFuncname();
            int lineNum = pBlockHeader->GetLineNum();
            if(typeName )// && 0 != strcmp(typeName, "[unknown]"))
            {
                fprintf(fp, "*** #%-6d %5d bytes %-50s\n", ++index, size, typeName);
                fprintf(fp, "... %s:%d:%s\n", fileName, lineNum, funcName);
            }
        }
        fprintf(fp, "-------------------------------------------------------------------\n");
        // Clean up.
        free(ppBlockHeader);
        fflush(fp);
        fclose(fp);

        fp = fopen("MemoryBlocks.txt", "r");
        if(fp)
        {   while(!feof(fp))
            {
                char szLine[1024] = {0};
                fgets(szLine, 1024, fp);
                printf(szLine);
            }
            fclose(fp);
        }
    }

    /* ---------------------------------------- struct MemDigest */
    struct FMemDigest
    {
        char const *typeName;
        int blockCount;
        size_t totalSize;

        static bool TotalSizeGreaterThan(const FMemDigest &md1, const FMemDigest &md2)
            { return md1.totalSize > md2.totalSize; }
    };

    /* ---------------------------------------- SummarizeMemoryUsageForType */
    inline void FSummarizeMemoryUsageForType(FMemDigest *pMemDigest, FBlockHeader **ppBlockHeader, size_t startPost, size_t endPost)
    {
        pMemDigest->typeName = ppBlockHeader[startPost]->GetTypeName();
        pMemDigest->blockCount = 0;
        pMemDigest->totalSize = 0;
        for (size_t i = startPost; i < endPost; i++)
        {
            pMemDigest->blockCount++;
            pMemDigest->totalSize += ppBlockHeader[i]->GetRequestedSize();
            assert(strcmp(ppBlockHeader[i]->GetTypeName(), pMemDigest->typeName) == 0);
        }
    }

    /* ---------------------------------------- TrackListMemoryUsage */
    inline void FTrackListMemoryUsage()
    {
        // If there are no allocated blocks, then return now.
        size_t numBlocks = FBlockHeader::BlockHeaderStatic::Get().CountBlocks();
        if (numBlocks == 0) return;

        // Get an array of pointers to all extant blocks.
        FBlockHeader **ppBlockHeader =
            (FBlockHeader **)calloc(numBlocks, sizeof(*ppBlockHeader));
        FBlockHeader::BlockHeaderStatic::Get().GetBlocks(ppBlockHeader);

        // Sort the blocks by type name.
        std::sort(
            ppBlockHeader,
            ppBlockHeader + numBlocks,
            FBlockHeader::TypeGreaterThan
        );

        // Find out how many unique types we have.
        size_t numUniqueTypes = 1;
        for (size_t i = 1; i < numBlocks; i++)
        {
            char const *prevTypeName = ppBlockHeader[i - 1]->GetTypeName();
            char const *currTypeName = ppBlockHeader[i]->GetTypeName();
            if (strcmp(prevTypeName, currTypeName) != 0) numUniqueTypes++;
        }

        // Create an array of "digests" summarizing memory usage by type.
        size_t startPost = 0;
        size_t uniqueTypeIndex = 0;
        FMemDigest *pMemDigestArray =
            (FMemDigest *)calloc(numUniqueTypes, sizeof(*pMemDigestArray));
        for (size_t i = 1; i <= numBlocks; i++)    // yes, less than or *equal* to
        {
            char const *prevTypeName = ppBlockHeader[i - 1]->GetTypeName();
            char const *currTypeName = (i < numBlocks) ? ppBlockHeader[i]->GetTypeName() : "";
            if (strcmp(prevTypeName, currTypeName) != 0)
            {
                size_t endPost = i;
                FSummarizeMemoryUsageForType(
                    pMemDigestArray + uniqueTypeIndex,
                    ppBlockHeader,
                    startPost,
                    endPost
                );
                startPost = endPost;
                uniqueTypeIndex++;
            }
        }
        assert(uniqueTypeIndex = numUniqueTypes);

        // Sort the digests by total memory usage.
        std::sort(
            pMemDigestArray,
            pMemDigestArray + numUniqueTypes,
            FMemDigest::TotalSizeGreaterThan
        );

        // Compute the grand total memory usage.
        size_t grandTotalNumBlocks = 0;
        size_t grandTotalSize = 0;
        for (size_t i = 0; i < numUniqueTypes; i++)
        {
            grandTotalNumBlocks += pMemDigestArray[i].blockCount;
            grandTotalSize += pMemDigestArray[i].totalSize;
        }

        // Dump the memory usage statistics.
        printf("\n");
        printf("-------------------------------------------------------------------\n");
        printf("                     Memory Usage Statistics                       \n");
        printf("-------------------------------------------------------------------\n");
        printf("\n");
        printf("%-50s%5s  %5s %7s %s \n", "allocated type", "blocks", "", "bytes", "");
        printf("%-50s%5s  %5s %7s %s \n", "--------------", "------", "", "-----", "");

        for (size_t i = 0; i < numUniqueTypes; i++)
        {
            FMemDigest *pMD = pMemDigestArray + i;
            size_t blockCount = pMD->blockCount;
            double blockCountPct = 100.0 * blockCount / grandTotalNumBlocks;
            size_t totalSize = pMD->totalSize;
            double totalSizePct = 100.0 * totalSize / grandTotalSize;

            printf(
                "%-50s %5d %5.1f%% %7d %5.1f%%\n",
                pMD->typeName,
                blockCount,
                blockCountPct,
                totalSize,
                totalSizePct
            );
        }
        printf("%-50s %5s %5s  %7s %s \n", "--------", "-----", "", "-------", "");
        printf("%-50s %5d %5s  %7d %s \n", "[totals]", grandTotalNumBlocks, "", grandTotalSize, "");

        // Clean up.
        free(ppBlockHeader);
        free(pMemDigestArray);
    }

    class FMemWatcher
    {
    public:
        ~FMemWatcher()
        {
            FTrackDumpBlocks(); 
            FTrackListMemoryUsage();
        }
    };

    static FMemWatcher _mem_watcher;
#endif


#undef FPAD_TO_ALIGNMENT_BOUNDARY
}

_FStdBegin

void FMemory::FMemStamp::AllocStamp(void* p, char const* typeName) const
{
	FAllocStamp(p, *this, typeName);
}


void* FMemory::Malloc(size_t size, const char* file, const char* func, int line)
{
	return FMalloc(size, file, func, line);
}
void* FMemory::Realloc(void* p, size_t size, const char* file, const char* func, int line)
{
	return FRealloc(p, size, file, func, line);
}
void* FMemory::Calloc(size_t nmemb, size_t size, const char* file, const char* func, int line)
{
	return FCalloc(nmemb, size, file, func, line);
}
void FMemory::Free(void* p)
{
	FFree(p);
}

_FStdEnd