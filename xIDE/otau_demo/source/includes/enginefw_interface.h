/******************************************************************************
 *  Copyright 2015 Qualcomm Technologies International, Ltd.
 *
 *  FILE
 *      enginefw_interface.h
 *
 *  DESCRIPTION
 *      The interface to the "engine framework"; a mechanism by which the tool
 *      manage debug tracing and user messages in a consistent manner.
 *
 ******************************************************************************/

#ifndef ENGINEFW_INTERFACE_H
#define ENGINEFW_INTERFACE_H

#ifndef NOCSR
#include "memoryleakcheck.h"
#endif // NOCSR

#include "types.h"
#include "ichar.h"

#include <string>
#include <list>

#ifndef NOCSR
#define FORCE_MLC_OFF
#include "memoryleakcheck.h"
#endif // NOCSR

#include <map>

#ifndef NOCSR
#define FORCE_MLC_ON
#include "memoryleakcheck.h"
#endif // NOCSR

#ifndef EF_GROUP
#define EF_GROUP CMessageHandler::GROUP_ENUM_RSVD_ALL
#endif

#ifdef WIN32
#ifdef ENGINEFRAMEWORKCPP_EXPORTS
#define ENGINEFRAMEWORKCPP_API          __declspec(dllexport) 
#else
#define ENGINEFRAMEWORKCPP_API          __declspec(dllimport)
#endif
#else
#define ENGINEFRAMEWORKCPP_API
#endif

/////////////////////////////////////////////////////////////////////////////

class IUnmanagedObserver
{
public:
    virtual ~IUnmanagedObserver() { }
	virtual void OnResponse(const std::string& aResponse) = 0;
};

class ENGINEFRAMEWORKCPP_API IUnmanaged
{
public:
    virtual ~IUnmanaged() { }
	virtual void SetObserver(IUnmanagedObserver& aObserver) = 0;
	virtual void PostRequest(const std::string& aRequest) = 0;
	virtual std::string SendRequest(const std::string& aRequest) = 0;
};

// Forward declarations
class CProgressMsg;
class CDebugMsg;
class CStatusMsg;
class CErrorMessages;
class CMessageHandlerObserver;
class AtomicCounter;
class CriticalSection;

// Note that DebugLevels and StatusLevels are not part of the CMessageHandler class because if they were,
// it would require prefixing every single use of them with "CMessageHandler::" and that is cumbersome.

/// The levels of debug messages available.
enum DebugLevels
{
    DEBUG_ENTRY_EXIT,   ///< Used internally to denote entry and exit to/from a method.
    DEBUG_PARAMETER,    ///< Used to express the value of an individual parameter to a method.
    DEBUG_ENHANCED,     ///< Used to express debug output that would be useful to someone who is debugging a complicated problem.
    DEBUG_BASIC,        ///< Used to express debug output that would be useful to someone who is starting to debug the code.
    DEBUG_ALL,          ///< Used only when turning levels on and off; do not use when outputting individual lines of debug.
    DEBUG_PROFILE,      ///< Used to insert a "profile point" in to the debug output
    DEBUG_COUNT_DO_NOT_USE_THIS ///< Used to make sure the enum is within bounds - NOT FOR USE OUTSIDE ENGINE FRAMEWORK!
};

/// The levels of status messages available.
enum StatusLevels
{
    STATUS_INFO,        ///< Informational message which the user would find useful to know.
    STATUS_ESSENTIAL,   ///< Information that the user has specifically asked for.
    STATUS_WARNING,     ///< Something that should be brought to the attention of the user, but not fatal.
    STATUS_ERROR,       ///< Something that is a fatal error and the situation cannot be resolved.
    STATUS_ALL,         ///< Used only when turning levels on and off; do not use when outputting individual lines of status.
    STATUS_COUNT_DO_NOT_USE_THIS ///< Used to make sure the enum is within bounds - NOT FOR USE OUTSIDE ENGINE FRAMEWORK!
};

/////////////////////////////////////////////////////////////////////////////

/// A generic subject to be observed.
class ENGINEFRAMEWORKCPP_API CSubject
{
public:
    virtual ~CSubject();
    virtual void Attach(CMessageHandlerObserver*);
    virtual void Detach(CMessageHandlerObserver*);
    virtual void Notify(uint32 supplementaryNumber);

protected: 
    CSubject();

private:
#ifdef WIN32
    // Because this data is private (and the user of the DLL cannot access it anyway),
    // the warning (cannot access data without making it part of the API) may safely be ignored.
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    std::list<CMessageHandlerObserver*> observers;
#ifdef WIN32
#pragma warning(pop)
#endif
};

/////////////////////////////////////////////////////////////////////////////
//                          CMessageHandler
/// @note Use MSG_HANDLER.xxxxxx() to access the methods in this class (where xxxxxx
/// is the method name described here).
/// @note Use MSG_HANDLER_NOTIFY_DEBUG() to write debug trace messages.
/// @note The engine framework will automatically shutdown when main is out of scope.
/// This means all calls will be ignored in the destructor of static objects.
/////////////////////////////////////////////////////////////////////////////

class ENGINEFRAMEWORKCPP_API CMessageHandler : public CSubject
{
public:
    /// The message type; used when an observer is being informed about a
    /// message having occurred.
    enum MessageType
    {
        MESSAGE_TYPE_PROGRESS,
        MESSAGE_TYPE_DEBUG,
        MESSAGE_TYPE_STATUS,
        MESSAGE_TYPE_COUNT
    };

    /// The group type - for use when manipulating how messages are processed.
    // **********************************************************************
    // ******************** I M P O R T A N T   N O T E *********************
    // **********************************************************************
    //  When adding a new entry here, an entry should also be added to to the 
    //  EF_GROUP_INFO structure.
    // **********************************************************************
    // **********************************************************************
    enum GroupEnum
    {
        // The groups that map directly to a specific LIB/DLL...
        // (where the library matches the binary output file unless otherwise stated) 
        GROUP_ENUM_PTTRANSPORT_LIB,
        GROUP_ENUM_UENERGY_LIB, ///< uEnergyCsEngine and uEnergyTest
        GROUP_ENUM_CMDLINE_LIB,
        GROUP_ENUM_A11EPROMCFGRW_LIB,
        GROUP_ENUM_CNDTEXT_LIB,
        GROUP_ENUM_CONFIGTOOLSCOREFRAMEWORK_LIB,
        GROUP_ENUM_CONFTOOLFWMANAGED_LIB,
        GROUP_ENUM_CURATORLIBRARY_LIB,
        GROUP_ENUM_CURATORWR_LIB,
        GROUP_ENUM_E2_LIB,
        GROUP_ENUM_FLASH_LIB,
        GROUP_ENUM_HYDRATEXT_LIB,
        GROUP_ENUM_PSHELP_LIB,
        GROUP_ENUM_SQLITEMETADATAPROVIDER_LIB,
        GROUP_ENUM_TESTE2_LIB,
        GROUP_ENUM_TESTFLASH_LIB,
        GROUP_ENUM_TESTENGINE_LIB,
        GROUP_ENUM_VLINTLIBRARY_LIB,
        GROUP_ENUM_NVSENGINE_LIB,
        GROUP_ENUM_BCCMDTRANS_LIB,
        GROUP_ENUM_CHUNKSIOIMPL_LIB,
        GROUP_ENUM_KEYFILE_LIB,
        GROUP_ENUM_SPIUNLOCK_LIB,
        GROUP_ENUM_HCITRANSPORT_LIB,
        GROUP_ENUM_PUNITEST_LIB,
        GROUP_ENUM_UNIPSLIB_LIB,
        GROUP_ENUM_HYDRAPARSER_LIB,
        GROUP_ENUM_PTAP_LIB,
        GROUP_ENUM_UNITEST_LIB,
        GROUP_ENUM_UNITESTHYDRA_LIB,
        GROUP_ENUM_COEXVALIDATOR_LIB,
        GROUP_ENUM_DFUIMAGE_LIB,
        GROUP_ENUM_HYDRAPROTOCOLS_LIB,
        GROUP_ENUM_FSIF_LIB,
        GROUP_ENUM_HYDRAEEPROM_LIB,
        GROUP_ENUM_HCIPACKER_LIB,
        GROUP_ENUM_ACLENGINE_LIB,
        GROUP_ENUM_ACCMD_LIB,

        // ADD NEW ENTRIES ***IMMEDIATELY*** BEFORE THIS LINE...
        // PLEASE READ THE NOTE ABOVE...

        // The generic groups to which code can belong...
        GROUP_ENUM_APPLICATION = 100, ///< The actual application (i.e. the implementation of main and closely related functions)
        GROUP_ENUM_PTTRANS_PLUGIN,    ///< PtTransport Plug-ins
        GROUP_ENUM_TEST_CODE,         ///< Test code (i.e. unit tests, regression tests etc.)
        GROUP_ENUM_UTILITY,           ///< Simple utility functions (often generic)

        /// @note Code cannot belong to the groups with "..._RSVD_..." in the
        /// name, but the enum values are used to manipulate messages.
        GROUP_ENUM_RSVD_ALL = 120,    ///< Used to denote "all groups"
        GROUP_ENUM_RSVD_LOCAL         ///< Used to denote "just the current group"
    };

    /// A structure to hold the details corresponding to a group enum value.
    struct GroupEnumInfoStruct
   {
        GroupEnum group;
        const char* name;
    };

    explicit CMessageHandler(GroupEnum aGroupName);
    virtual ~CMessageHandler();

    /////////////////////////////////////////////////////////////////////////

    ///
    /// The method to use to send progress indications of a specific percentage.
    /// @param[in] aValue The percentage level (a value between 0 and 100 inclusive).
    ///
    void NotifyProgress(uint16 aValue);

    ///
    /// A declaration for the method to use to send debug messages.
    /// @param[in] aLevel The debug level; any value except DEBUG_ALL.
    /// @param[in] aFormat The printf-style format, followed by the printf-style parameters.
    /// @note The method is never defined, always use the MSG_HANDLER_NOTIFY_DEBUG() pre-processor macro instead.
    /// It is only declared here so there is an entry in the "auto-complete" box to guide developers.
    ///
    void NOTIFY_DEBUG(DebugLevels aLevel, char* aFormat, ...);

    ///
    /// The method to use to send status messages.
    /// @param[in] aLevel The status level; any value except STATUS_ALL.
    /// @param[in] aText The text to be sent.
    ///
    void NotifyStatus(StatusLevels aLevel, const std::string& aText);

    /////////////////////////////////////////////////////////////////////////

    ///
    /// Set the error message, replacing the current error message.
    /// @param[in] aErrorCode The error code number shown to the user, unless
    /// set to the value zero (in which case there is no concept of an error number).
    /// @param[in] aText The text to be sent.
    /// @return true if the error message is stored, false otherwise.
    /// @note The concept is that there is only one error message in force at any one time.
    ///
    bool SetErrorMsg(int16 aErrorCode, const std::string& aText);

    ///
    /// Modify the error message, by prefixing the text to the start of the current error message.
    /// @param[in] aErrorCode Change the current error code to the value specified,
    /// unless zero is specified in which case the current code is left alone.
    /// @param[in] aText The text to be prefixed.
    /// @param[in] aGroupName The group name to use when searching for an existing error.
    /// Specify GROUP_ENUM_RSVD_LOCAL to look in the 'local' handler,
    /// GROUP_ENUM_RSVD_ALL to look in all handlers or a specific (other) enumeration
    /// to look in that specific handler. The new, modified error condition will only be assigned
    /// to the LOCAL group; this value is only used to determine the search criteria.
    /// @return true if the error message is stored, false otherwise.
    /// @note The concept is that there is only one error message in force at any one time.
    ///
    bool ModifyErrorMsg(int16 aErrorCode, const std::string& aText, GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL);

    ///
    /// Determine whether error messages are automatically transmitted to the user.
    /// @param[in] aNewAutomaticDisplay true if messages are to be automatically
    /// transmitted to the user, false otherwise.
    /// @param[in] aGroupName The group name to which the modifications apply.
    /// @return The value of the flag when the method was called (if the group was
    /// not GROUP_ENUM_RSVD_ALL, in which case it is always true).
    ///
    bool SetErrorAutomaticDisplay(bool aNewAutomaticDisplay, GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL);

    ///
    /// Retrieve the last/current error message details.
    /// @param[out] aErrorCode The code number of the error.
    /// @param[out] aText The text of the error.
    /// @param[in,out] aGroupName The group name in which to search.
    /// Specify GROUP_ENUM_RSVD_LOCAL to look in the 'local' handler,
    /// GROUP_ENUM_RSVD_ALL to look in all handlers or a specific (other) enumeration
    /// to look in that specific handler.
    /// On exit (if the return code is true) it contains the group that has the error.
    /// @param[in] bool aCurrentThreadOnly true to only look at data for this current thread;
    /// false for all threads.
    /// @return true if there is a current error and the ouput parameters have been populated,
    /// false otherwise (in which case the ouput parameters remain unchanged).
    ///
    bool LastError(int16& aErrorCode, std::string& aText, GroupEnum& aGroupName, bool aCurrentThreadOnly = true);

    ///
    /// Determine if there is a current error.
    /// @param[in] aGroupName The group name to use when searching for an existing error.
    /// @param[in] bool aCurrentThreadOnly true to only look at data for this current thread;
    /// false for all threads.
    /// @return true if there is a current error, false otherwise.
    ///
    bool IsErrorSet(GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL, bool aCurrentThreadOnly = true);

    ///
    /// Determine if the specified debug level is enabled in any handler
    /// @param[in] aLevel The level for which the settings are to be determined.
    /// @return true if the level is enabled in any handler, false otherwise.
    ///
    bool IsDebugLevelEnabled(DebugLevels aLevel);

    ///
    /// Clear the current error.
    /// @param[in] aGroupName The group name to which the modifications apply.
    ///
    void ClearError(GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL);

    /////////////////////////////////////////////////////////////////////////

    ///
    /// Set the current level at which debug messages are stored.
    /// @param[in] aLevel The level for which the settings are to be modified.
    /// @param[in] aEnable true to enable the level(s), false to disable.
    /// @param[in] aGroupName The group name to which the modifications apply.
    ///
    void SetDebugLevel(DebugLevels aLevel, bool aEnable, GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL);

    ///
    /// Set the current level at which status messages are stored.
    /// @param[in] aLevel The level for which the settings are to be modified.
    /// @param[in] aEnable true to enable the level(s), false to disable.
    /// @param[in] aGroupName The group name to which the modifications apply.
    ///
    void SetStatusLevel(StatusLevels aLevel, bool aEnable, GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL);

    /////////////////////////////////////////////////////////////////////////

    ///
    /// Create listeners for the debug object and send any debug enabled to stderr.
    ///
    void SendDebugOutputToTheConsole();

    ///
    /// Create listeners for all the message types (except debug) and write all appropriate
    /// messages to stdout.
    ///
    void SendAllNonDebugOutputToTheConsole();

    ///
    /// Create a listener for debug messages and write all appropriate
    /// messages to the specified stream.
    /// @param[in] aStream The stream to which all appropriate messages are to be written.
    /// @param[in] aGroupName The group name to which the modifications apply.
    /// @note The "NewXxxxObject" methods have a default group of "all" (the usual default is "local").
    /// However, it is highly probable that "all" is required here, so there is a different default..
    ///
    void NewDebugObject(std::ostream* aStream, GroupEnum aGroupName = GROUP_ENUM_RSVD_ALL);

    ///
    /// Create a listener for status messages and write all appropriate
    /// messages to the specified stream.
    /// @param[in] aStream The stream to which all appropriate messages are to be written.
    /// @param[in] aGroupName The group name to which the modifications apply.
    /// @note The "NewXxxxObject" methods have a default group of "all" (the usual default is "local").
    /// @note Error messages (if also initialised) are also written to this stream.
    ///
    void NewStatusObject(std::ostream* aStream, GroupEnum aGroupName = GROUP_ENUM_RSVD_ALL);

    ///
    /// Create a listener for error messages.
    /// @param[in] aAutomaticDisplay @see SetErrorAutomaticDisplay.
    /// @param[in] aGroupName The group name to which the modifications apply.
    /// @note The "NewXxxxObject" methods have a default group of "all" (the usual default is "local").
    /// @note An error object is now automatically created for each group,
    /// so this call is effectively redundant and equivalent to SetErrorAutomaticDisplay.
    /// @see NewStatusObject.
    ///
    void NewErrorObject(bool aAutomaticDisplay, GroupEnum aGroupName = GROUP_ENUM_RSVD_ALL);

    ///
    /// Create a listener for progress messages.
    /// @param[in] aGroupName The group name to which the modifications apply.
    ///
    void NewProgressObject(GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL);

    ///
    /// Add an observer for messages.
    /// @param[in] aGroupName The group name to which the modifications apply.
    /// @note It is indended that multiple observers can be added, but for the
    /// time being, only the last observer to be added will be informed of messages.
    ///
    void AddObserver(CMessageHandlerObserver& aObserver, GroupEnum aGroupName = GROUP_ENUM_RSVD_LOCAL);

    /////////////////////////////////////////////////////////////////////////

    ///
    /// Get the current "paragraph string".
    /// Any (status or error) messages might need to be formatted differently
    /// when displayed to the user depending on whether the user is operating
    /// a GUI, a console application or a DLL. Use the value supplied by this
    /// method to break up messages in to multiple "sections" (for example
    /// when there is a context change in the message).
    /// @return A string to be used between "sections" of a message.
    ///
    const std::string GetParagraphString() const;

    ///
    /// Set the current "paragraph string".
    /// @param[in] aNewParagraphString @see GetParagraphString().
    ///
    void SetParagraphString(const std::string& aNewParagraphString);

    /////////////////////////////////////////////////////////////////////////

    /// @note Not intended for general use - internal to the engine
    /// framework but still needs to be public to remain accessible.
    static std::string FindClassName(const std::string& aFnName);

    /// @note Not intended for general use - internal to the engine
    /// framework but still needs to be public to remain accessible.
    static std::string DetermineQualifiedFnName(
        const char* aDecoratedFnName, bool aStripTemplateParameters);

    /// @note Not intended for general use - internal to the engine
    /// framework but still needs to be public to remain accessible.
    void DebugOutputBuffer(
        uint32 aLevel, const char* aFunction, const char* aFilename,
        uint32 aLineNum, const char* aDescription, uint8* apBuffer,
        uint32 aLength);

    /// @note Not intended for general use - internal to the engine
    /// framework but still needs to be public to remain accessible.
    void DebugOutputProfile(uint32 aProfileNumber);

    /// @note Not intended for general use - internal to the engine
    /// framework but still needs to be public to remain accessible.
    /// @note Do not use this function directly; use the
    /// MSG_HANDLER_NOTIFY_DEBUG() macro instead.
    void DebugOutputInHandler(
        uint32 aLevel, bool aEntry, bool aExit, const char* aFunction,
        const char* aFilename, uint32 aLineNum, const char* aFormat, ...);

    /// @note Not intended for general use - internal to the engine
    /// framework but still needs to be public to remain accessible.
    void Initialise();

    /// @note Not intended for general use - internal to the engine
    /// framework but still needs to be public to remain accessible.
    void Shutdown();

    // Legacy methods to make the DLL backwards-compatible...
    CMessageHandler();

private:
    /// The maximum number allowed in the MSG_HANDLER_NOTIFY_PROFILE macro
    static const uint32 MAX_VALUE_FOR_PROFILE_NUMBER = 9;

    /// Used to determine within an iteration round the list of handlers whether the handler should be called or not.
    inline bool IsHandlerToBeActioned(GroupEnum aRequestedGroup, GroupEnum aLocalGroup, GroupEnum aThisGroup)
    {
        return (aRequestedGroup == GROUP_ENUM_RSVD_ALL ||
                aRequestedGroup == aThisGroup          ||
               (aRequestedGroup == GROUP_ENUM_RSVD_LOCAL && aThisGroup == aLocalGroup));
    }

    /// @return The string representation of this message handler.
    std::string GetDecoratedGroupName();

    CProgressMsg*   mpProgressMsg;
    CDebugMsg*      mpDebugMsg;
    CStatusMsg*     mpStatusMsg;
    CErrorMessages* mpErrorMsgs;
    bool            mAutomaticallyDisplayErrorMessages;
    GroupEnum       mGroupName;
#ifdef WIN32
    // Because this data is private (and the user of the DLL cannot access it anyway),
    // the warning (cannot access data without making it part of the API) may safely be ignored.
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    std::string   mParagraphString;
#ifdef WIN32
#pragma warning(pop)
#endif
};

/////////////////////////////////////////////////////////////////////////////
//                          CMessageHandlerObserver
/////////////////////////////////////////////////////////////////////////////

class ENGINEFRAMEWORKCPP_API CMessageHandlerObserver
{
public:
    virtual ~CMessageHandlerObserver();

    ///
    /// Used to notify an observer that an item of text has been sent.
    ///
    virtual void NotifyText(uint32 aRsvdForFuture, CMessageHandler::MessageType aType,
        uint32 aMinor, const char* apString) = 0;

    ///
    /// Used to notify an observer that a number has been sent.
    ///
    virtual void NotifyNumber(uint32 aRsvdForFuture, CMessageHandler::MessageType aType,
        uint32 aMinor, int32 aNumber) = 0;

protected:
    CMessageHandlerObserver();
};

/////////////////////////////////////////////////////////////////////////////
//                          CDebugSentry
/////////////////////////////////////////////////////////////////////////////

class ENGINEFRAMEWORKCPP_API CDebugSentry
{
public:
    CDebugSentry(CMessageHandler* apMsgHandler, uint32 aLineNum, const char* apFileName, const char* apFunctionName);
    virtual ~CDebugSentry();

protected:
    CMessageHandler* mpMsgHandlerPtr;
    uint32           mLineNum;
    bool             mExitPrinted;

#ifdef WIN32
    // Because this data is protected (and the user of the DLL cannot access it anyway),
    // the warning (cannot access data without making it part of the API) may safely be ignored.
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    std::string      mFileName;
    std::string      mFunctionName;
#ifdef WIN32
#pragma warning(pop)
#endif

    void CheckPrintExit();

    virtual void PrintExit();
};

/////////////////////////////////////////////////////////////////////////////
//                          CDebugSentryWithReturn
/////////////////////////////////////////////////////////////////////////////

#define DO_PRINT_EXIT(SUFFIX_PRINTF_FMT_STRING, SUFFIX_ITEM_VALUE)           \
    if (mExitPrinted == false)                                               \
    {   mpMsgHandlerPtr->DebugOutputInHandler(DEBUG_ENTRY_EXIT, false, true, \
            mFunctionName.c_str(), mFileName.c_str(), mLineNum,              \
            "<- %s" SUFFIX_PRINTF_FMT_STRING, mFunctionName.c_str(),         \
            (SUFFIX_ITEM_VALUE));                                            \
        mExitPrinted = true;                                                 \
    }

template <typename ReturnType> class CDebugSentryWithReturn : public CDebugSentry
{
public:
    CDebugSentryWithReturn(
        CMessageHandler* apMsgHandler,
        uint32 aLineNum,
        const char* apFileName,
        const char* apFunctionName,
        const ReturnType& aRet) :
        CDebugSentry(apMsgHandler, aLineNum, apFileName, apFunctionName), mRet(aRet)
    {
    }

    virtual ~CDebugSentryWithReturn()
    {
        CheckPrintExit();
    }

private:
    const ReturnType &mRet;

    virtual void PrintExit()
    {
        DoPrintExit(mRet);
    }

    void DoPrintExit(bool aRet)
    {
        DO_PRINT_EXIT(" [returning %s]", (aRet ? "true" : "false") );
    }

    void DoPrintExit(int aRet)
    {
        DO_PRINT_EXIT(" [returning %d]", aRet);
    }

#if defined(WIN64) || defined(__LP64__)
    void DoPrintExit(unsigned int aRet)
#else
    void DoPrintExit(size_t aRet)
#endif
    {
        DO_PRINT_EXIT(" [returning %u]", aRet);
    }

    void DoPrintExit(long aRet)
    {
        DO_PRINT_EXIT(" [returning %ld]", aRet);
    }

    void DoPrintExit(unsigned long aRet)
    {
        DO_PRINT_EXIT(" [returning %lu]", aRet);
    }

    void DoPrintExit(int64 aRet)
    {
        DO_PRINT_EXIT(" [returning %lld]", aRet);
    }

    void DoPrintExit(uint64 aRet)
    {
        DO_PRINT_EXIT(" [returning %llu]", aRet);
    }

    void DoPrintExit(const char *aRet)
    {
        DO_PRINT_EXIT(" [returning \"%s\"]", aRet);
    }

    void DoPrintExit(void *aRet)
    {
        DO_PRINT_EXIT(" [returning 0x%08p]", aRet);
    }

    void DoPrintExit(const std::string& aRet)
    {
        DO_PRINT_EXIT(" [returning \"%s\"]", aRet.c_str());
    }

    void DoPrintExit(const std::wstring& aRet)
    {
        DoPrintExit(inarrow(aRet));
    }

    void DoPrintExit(float aRet)
    {
        DO_PRINT_EXIT(" [returning %f]", aRet);
    }

    void DoPrintExit(double aRet)
    {
        DO_PRINT_EXIT(" [returning %lf]", aRet);
    }
};

/////////////////////////////////////////////////////////////////////////////
//                          CEngineInitialise
/////////////////////////////////////////////////////////////////////////////

class ENGINEFRAMEWORKCPP_API CEngineInitialise
{
public:
    CEngineInitialise();
    virtual ~CEngineInitialise();
};

/////////////////////////////////////////////////////////////////////////////
//                          CTheMsgHnd
/////////////////////////////////////////////////////////////////////////////

class ENGINEFRAMEWORKCPP_API CTheMsgHnd
{
    /// @note Do not access this class directly; only use the pre-processor
    /// macros provided!

private:
    friend class CEngineInitialise;
    friend class CMessageHandler;

    CTheMsgHnd();
    virtual ~CTheMsgHnd();
    void CreateGroups(int aLowerIndex, int aUpperIndex);
    void CreateGroup(CMessageHandler::GroupEnum aGroupName);

    // Used internally within the family of engine framework clases
    // (i.e. NOT by calling code) to determine the ORDERING of errors.
    // Do not make this public because the number will be meaningless to
    // users of the library.
    AtomicCounter* mpErrorIndex;

    /// A critical section object used to sync access by the objects
    /// This is at a very broad level, but it provides consistency between
    /// the base objects used within the framework.
    static CriticalSection* mpSynchroniseLock;

#ifdef WIN32
    // Because this data is private (and the user of the DLL cannot access it anyway),
    // the warning (cannot access data without making it part of the API) may safely be ignored.
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    static std::map<CMessageHandler::GroupEnum, CMessageHandler*> mTheHandlers;
    typedef std::map<CMessageHandler::GroupEnum, CMessageHandler*>::iterator MsgHndIter;

    static std::map<const std::string, CMessageHandler::GroupEnum> mTheGroups;
    typedef std::map<const std::string, CMessageHandler::GroupEnum>::iterator GroupsIter;
#ifdef WIN32
#pragma warning(pop)
#endif

public:
    static CTheMsgHnd& Instance();

    /// This globally accessible variable is used (as a speed optimisation) to quickly determine
    /// if debugging has been activated. It is a static member for speed optimisation purposes
    /// (even though the class is a singleton).
    /// @note DO NOT alter it in code outside the engine
    /// framework, but it nevertheless needs to remain publicly visible.
    /// Changing the value will not actually cause problems, but it will be meaningless
    /// and not achieve anything beneficial (it will just slow it down or deprive you
    /// of tracing when you ask for it).
    /// That was the ideal way of doing things, however the EngineFrameworkClr module requires
    /// a delay-loaded EngineFrameworkCpp. Delay loading precludes the use of public data!
    /// Doing it this way adds a fairly bad time penalty, run EngineExampleConsole -speedtest 10000
    /// 20uS with global variable, 250uS with function call
private:
    static bool mDebugActivated;
public:
    static bool DebugActivated() { return mDebugActivated; }

    CMessageHandler::GroupEnum GetLocalGroup(CMessageHandler* apThisMsgHandler);
    static CMessageHandler& GetAppropriateHandler(CMessageHandler::GroupEnum aGroupName, const char* apFnName, bool aCreateLink);
};

/////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define OS_AGNOSTIC_FUNCTION __FUNCTION__
#else
#define OS_AGNOSTIC_FUNCTION __PRETTY_FUNCTION__
#endif

/// Use this structure to obtain the equivalent string names of the groups.
//  Place CMDLINE first so that the debug output is displayed when turning groups on/off.
static const CMessageHandler::GroupEnumInfoStruct EF_GROUP_INFO[] = {
    { CMessageHandler::GROUP_ENUM_CMDLINE_LIB,       "CmdLine"         },
    // Generic ones...
    { CMessageHandler::GROUP_ENUM_APPLICATION,       "Application"     },
    { CMessageHandler::GROUP_ENUM_PTTRANS_PLUGIN,    "PtTransPlugIn"   },
    { CMessageHandler::GROUP_ENUM_TEST_CODE,         "TestCode"        },
    { CMessageHandler::GROUP_ENUM_UTILITY,           "Utility"         },
    // Specific DLLs and LIBs...
    { CMessageHandler::GROUP_ENUM_PTTRANSPORT_LIB,   "PtTransport"     },
    { CMessageHandler::GROUP_ENUM_UENERGY_LIB,       "uEnergy"         },
    { CMessageHandler::GROUP_ENUM_A11EPROMCFGRW_LIB, "A11EpromCfgRw"   },
    { CMessageHandler::GROUP_ENUM_CNDTEXT_LIB,       "CndText"         },
    { CMessageHandler::GROUP_ENUM_CONFIGTOOLSCOREFRAMEWORK_LIB, "ConfigToolsCoreFramework" },
    { CMessageHandler::GROUP_ENUM_CONFTOOLFWMANAGED_LIB, "ConfToolsFwManaged" },
    { CMessageHandler::GROUP_ENUM_CURATORLIBRARY_LIB, "CuratorLibrary" },
    { CMessageHandler::GROUP_ENUM_CURATORWR_LIB,      "CuratorWr"      },
    { CMessageHandler::GROUP_ENUM_E2_LIB,             "E2"             },
    { CMessageHandler::GROUP_ENUM_FLASH_LIB,          "Flash"          },
    { CMessageHandler::GROUP_ENUM_HYDRATEXT_LIB,      "HydraText"      },
    { CMessageHandler::GROUP_ENUM_PSHELP_LIB,         "PsHelp"         },
    { CMessageHandler::GROUP_ENUM_SQLITEMETADATAPROVIDER_LIB, "SQLiteMetadataProvider" },
    { CMessageHandler::GROUP_ENUM_TESTENGINE_LIB,     "TestEngine"     },
    { CMessageHandler::GROUP_ENUM_TESTE2_LIB,         "TestE2"         },
    { CMessageHandler::GROUP_ENUM_TESTFLASH_LIB,      "TestFlash"      },
    { CMessageHandler::GROUP_ENUM_VLINTLIBRARY_LIB,   "VLIntLibrary"   },
    { CMessageHandler::GROUP_ENUM_NVSENGINE_LIB,      "NvsEngine"      },
    { CMessageHandler::GROUP_ENUM_BCCMDTRANS_LIB,     "BcCmdTrans"     },
    { CMessageHandler::GROUP_ENUM_CHUNKSIOIMPL_LIB,   "ChunksIoImpl"   },
    { CMessageHandler::GROUP_ENUM_KEYFILE_LIB,        "KeyFile"        },
    { CMessageHandler::GROUP_ENUM_SPIUNLOCK_LIB,      "SpiUnlock"      },
    { CMessageHandler::GROUP_ENUM_HCITRANSPORT_LIB,   "HciTransport"   },
    { CMessageHandler::GROUP_ENUM_PUNITEST_LIB,       "pUniTest"       },
    { CMessageHandler::GROUP_ENUM_UNIPSLIB_LIB,       "UniPsLib"       },
    { CMessageHandler::GROUP_ENUM_HYDRAPARSER_LIB,    "HydraParser"    },
    { CMessageHandler::GROUP_ENUM_PTAP_LIB,           "PTap"           },
    { CMessageHandler::GROUP_ENUM_UNITEST_LIB,        "Unitest"        },
    { CMessageHandler::GROUP_ENUM_UNITESTHYDRA_LIB,   "UnitestHydra"   },
    { CMessageHandler::GROUP_ENUM_COEXVALIDATOR_LIB,  "CoexValidator"  },
    { CMessageHandler::GROUP_ENUM_DFUIMAGE_LIB,       "DfuImage"       },
    { CMessageHandler::GROUP_ENUM_HYDRAPROTOCOLS_LIB, "HydraProtocols" },
    { CMessageHandler::GROUP_ENUM_FSIF_LIB,           "Fsif"           },
    { CMessageHandler::GROUP_ENUM_HYDRAEEPROM_LIB,    "HydraEeprom"    },
    { CMessageHandler::GROUP_ENUM_HCIPACKER_LIB,      "HciPacker"      },
    { CMessageHandler::GROUP_ENUM_ACLENGINE_LIB,      "AclEngine"      },
    { CMessageHandler::GROUP_ENUM_ACCMD_LIB,          "Accmd"          },
};

// Delete this when applications have been updated and no longer use it...
#define ENGINE_FW_MAIN_INITIALISE

/// Use this to access the message handler.
#define MSG_HANDLER \
    CTheMsgHnd::Instance().GetAppropriateHandler(EF_GROUP, OS_AGNOSTIC_FUNCTION, false)

/// Use this (in the constructor) to add that class to the specified group.
#define MSG_HANDLER_ADD_TO_GROUP(GROUP_NAME) \
    CTheMsgHnd::Instance().GetAppropriateHandler((GROUP_NAME), OS_AGNOSTIC_FUNCTION, true)

// To completely disable debugging, define this macro.
// ONLY DO THIS AS A TEST AND NOT AS A MATTER OF COURSE.
// There was a debate as to whether to define this macro in a release build, but it was
// decided to leave debugging enabled in ALL builds (including release builds) and only
// turn debugging off to prove whether an observed problem somewhere was in any way 
// connected to the tracing mechanism.
//#define EF_DISABLE_ALL_DEBUG

#ifdef EF_DISABLE_ALL_DEBUG

#define MSG_HANDLER_NOTIFY_DEBUG(X, ...)
#define MSG_HANDLER_NOTIFY_DEBUG_BUFFER(X, ...)
#define MSG_HANDLER_NOTIFY_PROFILE_POINT(X)
#define FUNCTION_DEBUG_SENTRY 
#define FUNCTION_DEBUG_SENTRY_RET(X, ...)

#else

/// Use this to write a line of debug.
#define MSG_HANDLER_NOTIFY_DEBUG(LEVEL, FORMAT, ...) \
    if (CTheMsgHnd::DebugActivated())                 \
    { MSG_HANDLER.DebugOutputInHandler( (LEVEL), false, false, OS_AGNOSTIC_FUNCTION, __FILE__, __LINE__, (FORMAT), ##__VA_ARGS__); }

/// Use this to dump a buffer as debug.
#define MSG_HANDLER_NOTIFY_DEBUG_BUFFER(LEVEL, DESCRIPTION, UINT8PTR, LENGTH) \
    if (CTheMsgHnd::DebugActivated())                                          \
    { MSG_HANDLER.DebugOutputBuffer( (LEVEL), OS_AGNOSTIC_FUNCTION, __FILE__, __LINE__, (DESCRIPTION), const_cast<uint8*>((UINT8PTR)), (LENGTH) ); }

#define MSG_HANDLER_NOTIFY_PROFILE_POINT(NUMBER) \
    if (CTheMsgHnd::DebugActivated()) { MSG_HANDLER.DebugOutputProfile(NUMBER); }

/// Use this as the very first code inside every non-trivial method that returns void.
#define FUNCTION_DEBUG_SENTRY \
    CDebugSentry functionDebugPrinter( &(MSG_HANDLER) , __LINE__, __FILE__, OS_AGNOSTIC_FUNCTION )

/// Use this as the very first code inside every non-trivial non-void method (to automatically
/// print out the value of the return code on exit from the function). This does, of course,
/// rely on the function actually returning the variable that it gave to this macro.
#define FUNCTION_DEBUG_SENTRY_RET(RET_TYPE, RET_VALUE) \
    CDebugSentryWithReturn<RET_TYPE> functionDebugPrinter( &(MSG_HANDLER) , __LINE__, __FILE__, OS_AGNOSTIC_FUNCTION, (RET_VALUE) )

#endif

#endif
