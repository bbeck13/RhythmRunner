////////////////////////////////////////////////////////////////////////////////
///
///  glee.hpp
///  GLEE: OpenGL Error Extensions
///
////////////////////////////////////////////////////////////////////////////////
///
/// DESCRIPTION
///
///     GLEE is a set of error finding tools designed to plug in on top of
/// existing OpenGL code. Instead of racking your brain over where an OpenGL
/// error is occuring with assertions and error checks, include this file AFTER
/// you include OpenGL's headers, and most common functions will automatically
/// become guarded by a GLEE_GuardedGLCall macro invocation. This macro, which
/// calls GLEE::AssertNoOpenGLErrors before and after the OpenGL function call,
/// will crash the currently running program with an assert(false) if
/// glGetError() has something to say about your OpenGL state machine.
///
///     GLEE is best used with a debugger, as assert(false) calls will halt the
/// the debugger right at the error point and allow you to see where you are in
/// the stack trace. Additionally, with a network connection and GLEE_NETWORKING
/// defined, you can also get some handy info from the web brought directly to
/// std::err about the nature of the OpenGL failure.
///
///     GLEE compiles with GNU C++ (##__VA_ARGS__ is the culprit). So just about
/// every compiler supports it, and any warnings you get about GNU should be
/// able to be silenced by -Wgnu. 
///
/// USAGE
///
///     Most likely you include your OpenGL code directly:
///
///
///                          *----------*           *------------*
///                          |   gl.h   | <-------- |  Your App  |
///                          *----------*  includes *------------*
///
///
///     In order to achieve uniform function overwriting, insert glee
/// in-between your gl.h file (or glew.h, etc.) and your app. For example, you
/// could use the following configuration:
///
///
///         * -DGLEE_OVERWRITE_GL_FUNCTIONS
///         |        * -DGLEE_GL_HEADER_PATH=<OpenGL/gl.h>
///     *--------*   |       *----------*           *------------*
///     |  gl.h  | <-------- | glee.hpp | <-------- |  Your App  |
///     *--------*  includes *----------*  includes *------------*
///
///
/// AVAILABLE CONFIGURATIONS
///
///   The following are all optional with descriptions below:
///
///     o  GLEE_OVERWRITE_GL_FUNCTIONS
///         *  GLEE_CHECK_UNIFORM_LOCATIONS
///         *  GLEE_NETWORKING
///     o  GLEE_GL_HEADER_PATH
///
////////////////////////////////////////////////////////////////////////////////
///
/// TABLE OF CONTENTS
///     1. Customization
///     2. Includes
///     3. Error Checking Mechanisms
///     4. Networking
///     5. OpenGL Redifinitions
///

#ifndef glee_hpp
#define glee_hpp

////////////////////////////////////////////////////////////////////////////////
///
/// 1. Customization
///

/// macro: GLEE_OVERWRITE_GL_FUNCTIONS
///
/// DISCUSSION
///
///     Define this macro to enable automatic OpenGL error checking. Once this
/// file is included, it will automatically create shims around the listed
/// OpenGL functions below and call error checking functions to ensure that
/// your OpenGL code is working correctly and tells you exactly where and
/// how failures propagate.

//#define GLEE_OVERWRITE_GL_FUNCTIONS

/// macro: GLEE_CHECK_UNIFORM_LOCATIONS
///
/// DISCUSSION
///
///     Defining this adds a check for negative values sent to glUniform*
/// functions.

//#define GLEE_CHECK_UNIFORM_LOCATIONS

/// macro: GLEE_NETWORKING
///
/// DISCUSSION
///
///     Define this macro to enable OpenGL error checking to additionally
/// try and find the web page for the failing function and report in more
/// detail the nature of the failure directly in the commandline.
///
/// NOTES
///
///     Networking functionality requires that you add libxml2 to your include
/// path (i.e. -I/usr/local/include/libxml2 on macOS, -I/usr/include/libxml2 and
/// -I/usr/include in the CSL) and link to libcurl and libxml2 (i.e.
/// -L/usr/local/lib on macOS and -L/usr/lib in the CSL).

//#define GLEE_NETWORKING

/// include: GL.h
///
/// DISCUSSION
///
///     The user of this file must include OpenGL's header location for GLEE
/// to compile. Depending on your project configuration (for example, with
/// GLFW3), it may not be desirable to directly include the GL.h file. The
/// author recommends a dedicated header where you include OpenGL for all
/// files that use OpenGL include that file instead.

namespace GL_include {
    //i.e. #include "gl_include.h"
    //i.e. #include <OpenGL/gl.h>
    //i.e. #include <GL/glut.h>
    //i.e. #include <GL/glew.h>
}

/// macro: GLEE_GL_HEADER_PATH
///
///     If, instead of changing this file for the GL.h include, you prefer to
/// have a more generically configurable build, then define this macro. For
/// example, -DGLEE_GL_HEADER_PATH="<OpenGL/gl3.h>", or even just define the macro
/// in code for the same effect.

#if defined(GLEE_GL_HEADER_PATH)
namespace GL_include {
    #include GLEE_GL_HEADER_PATH
}
#endif // defined(GLEE_GL_HEADER_PATH)

////////////////////////////////////////////////////////////////////////////////
///
/// 2. Includes
///

/// Here we place all of the include OpenGL symbols in the same space as ::
using namespace GL_include;

/// Required headers

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <map>

//////

#ifdef GLEE_NETWORKING

#include <stdio.h>
#include <string.h>
#ifndef WIN32
#  include <sys/time.h>
#endif
#include <stdlib.h>
#include <errno.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#endif // GLEE_NETWORKING

#if defined(GLEE_CHECK_UNIFORM_LOCATIONS)
# define GLEE_DO_CHECK_UNIFORMS (1)
#else // !defined(GLEE_CHECK_UNIFORM_LOCATIONS)
# define GLEE_DO_CHECK_UNIFORMS (0)
#endif // defined(GLEE_CHECK_UNIFORM_LOCATIONS)
////////////////////////////////////////////////////////////////////////////////
struct glee_api {


////////////////////////////////////////////////////////////////////////////////
///
/// 3. Error Checking Mechanisms
///

#ifdef GLEE_OVERWRITE_GL_FUNCTIONS

    /// DESCRIPTION
    ///
    ///     Checks to see if any OpenGL errors exists, and if so the program will crash and an error will
    /// be printed out including the location of where this function was called. If `oglFuncName` is
    /// supplied, then OpenGLAPIDocRequester will be used to try and find the documentation for that
    /// function and print out the specific descriptions for the errors. Otherwise, the generic errors
    /// are printed instead. Ideally this function is called before and after every OpenGL API call
    /// in order to make debugging a lot easier.
    ///
    /// NOTES:
    ///
    ///     Call this to stop program execution if an OpenGL error has occurred. If an
    /// error has occurred, `message` will be displayed with the additionally
    /// formatted information: `functionName` (the function that called this assert),
    /// `fileName` (the file of the source where `functionName` was written),
    /// and `lineNumber` (the line file `fileName` in `functionName` that this assert
    /// call was made).
    ///
    ///     Optionally, the caller can specify `oglFuncName` to state which function they
    /// expected to be the source of the OpenGL error if one occurs. If an error does
    /// occur, a network query will be performed and will attempt to look up the error
    /// description on the OpenGL website.
    static void
    AssertNoOpenGLErrors(
        const std::string & message,
        const std::string & functionName,
        const std::string & fileName,
        const int lineNumber,
        const std::string & oglFuncName)
    {

        OpenGLStateErrorInfo stateErrors = GetOpenGLStateErrors();
        if (stateErrors.encounteredError) {
            
            std::cerr << "{" << fileName << ":" << lineNumber
             << " (" << functionName << ")} opengl assertion failure: "
             << message << std::endl << std::endl;
            
            if (oglFuncName.length() > 0) {
                std::string causeMessages = "";
                OpenGLAdditionalErrorInfo additionalInfo = RequestOpenGLAPIErrorInfoForFunction(oglFuncName);
                for (std::vector<OpenGLEnumInfo>::iterator itr  = stateErrors.rawErrors.begin();
                    itr != stateErrors.rawErrors.end(); itr++) {
                    
                    std::vector<std::string> errorStrings
                     = additionalInfo.errorsForEnumInfo(*itr);
                    for (std::vector<std::string>::iterator strItr = errorStrings.begin();
                        strItr != errorStrings.end(); strItr++) {
                        
                        causeMessages += *strItr;
                    }
                }
                
                /// General causes
                std::cerr << "General Information: " << stateErrors.description << std::endl << std::endl;
                
                if (causeMessages.length() > 0) {
                    /// Specific possible causes
                    std::cerr << "Possible causes: " << causeMessages << std::endl << std::endl;
                }
                
                if (additionalInfo.url.length() > 0) {
                    std::cerr << "For more information visit: " << additionalInfo.url << std::endl << std::endl;
                }
            }

            assert(false);
        }
    }
    
    ///
    static inline std::string
    joinStringsWithSeperator(
        const std::vector<std::string> & strings,
        const std::string & separator)
    {
        std::string result = "";
        for (std::vector<std::string>::const_iterator itr = strings.begin();
         itr != strings.end(); itr++) {
            result += *itr + separator;
        }
        return result;
    }

    /// Takes the enum value given by |glGetError| and turns it into a parsed
    /// string based on the descriptions for each error available here:
    /// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetError.xml
    struct OpenGLEnumInfo {
        
        /// The OpenGL error enumeration we seek to get information about.
        GLenum enumValue;
        
        /// Constructs an OpenGLEnumInfo with an enum value.
        OpenGLEnumInfo(GLenum _enumValue) : enumValue(_enumValue) {}
        
        /// Returns whether `enumValue` contains `value` in its bits.
        bool
        enumMatches(GLenum value) const
        {
            return enumValue == value;
        }
        
        
        /// The error message to append to any given error.
        static const std::string
        kGeneralErrorDescription()
        {
            return std::string("The offending command is ignored and has no other side effect than to set the error flag.");
        }
        
        /// The generic meaning to GL_INVALID_ENUM
        static const std::string
        kInvalidEnumDescription()
        {
            return std::string("(GL_INVALID_ENUM) An unacceptable value is specified for an enumerated argument.");
        }
        
        /// The generic meaning to GL_INVALID_VALUE
        static const std::string
        kInvalidValueDescription()
        {
            return std::string("(GL_INVALID_VALUE) A numeric argument is out of range.");
        }
        
        /// The generic meaning to GL_INVALID_OPERATION
        static const std::string
        kInvalidOperationDescription()
        {
            return std::string("(GL_INVALID_OPERATION) The specified operation is not allowed in the current state.");
        }
        
        /// The generic meaning to GL_INVALID_FRAMEBUFFER_OPERATION
        static const std::string
        kInvalidFramebufferOperationDescription()
        {
            return std::string("(GL_INVALID_FRAMEBUFFER_OPERATION) The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE).");
        }
        
        /// The generic meaning to GL_OUT_OF_MEMORY
        static const std::string
        kOutOfMemoryDescription()
        {
            return std::string("(GL_OUT_OF_MEMORY) There is not enough memory left to execute the command. The state of the GL is undefined.");
        }
        
        /// The meaning of GL_NO_ERROR
        static const std::string
        kNoErrorDescription()
        {
            return std::string("(GL_NO_ERROR) No error has been recorded.");
        }
        
        /// Whether this error contains GL_INVALID_ENUM
        bool
        invalidEnum() const
        {
            return enumMatches(GLenum(GL_INVALID_ENUM));
        }
        
        /// Whether this error contains GL_INVALID_VALUE
        bool
        invalidValue() const
        {
            return enumMatches(GLenum(GL_INVALID_VALUE));
        }
        
        /// Whether this error contains GL_INVALID_OPERATION
        bool
        invalidOperation() const
        {
            return enumMatches(GLenum(GL_INVALID_OPERATION));
        }
        
        /// Whether this error contains GL_INVALID_FRAMEBUFFER_OPERATION
        bool
        invalidFramebufferOperation() const
        {
            return enumMatches(GLenum(GL_INVALID_FRAMEBUFFER_OPERATION));
        }
        
        /// Whether this error contains GL_OUT_OF_MEMORY
        bool
        outOfMemory() const
        {
            return enumMatches(GLenum(GL_OUT_OF_MEMORY));
        }
        
        /// Whether this error is GL_NO_ERROR
        bool
        noError() const
        {
            return enumValue == GLenum(GL_NO_ERROR);
        }
        
        /// The concatenated result of each error and its generic description.
        std::string
        generalErrorDescription() const
        {
            std::string result = "";
            if (invalidEnum()) {
                result += kInvalidEnumDescription() + " ";
            }
            if (invalidValue()) {
                result += kInvalidValueDescription() + " ";
            }
            if (invalidOperation()) {
                result += kInvalidOperationDescription() + " ";
            }
            if (invalidFramebufferOperation()) {
                result += kInvalidFramebufferOperationDescription() + " ";
            }
            if (outOfMemory()) {
                result += kOutOfMemoryDescription() + " ";
            }
            
            if (noError()) {
                result += kNoErrorDescription();
            }
            else {
                result += kGeneralErrorDescription();
            }
            
            return result;
        }
    };

    struct OpenGLStateErrorInfo {
        bool encounteredError;
        std::string description;
        std::vector<OpenGLEnumInfo> rawErrors;
    };

    /// A wrapper struct used to collect specific information about a particular OpenGL function.
    /// In particular, the list of errors for each type is given and then can be used in other areas.
    /// The intention of this class is to be used with OpenGLAPIDocRequester to encapsulate, in a 
    /// generic way, the data present in the OpenGL documentation.
    struct OpenGLAdditionalErrorInfo {

        /// The name of the function that each error property refers to.
        std::string functionName;
        ///
        std::string url;
        ///
        std::vector<std::string> errorNotes;

        /// A map of various OpenGL errors and a list of error descriptions that accompany each.
        std::map< GLenum, std::vector<std::string> > errorDescriptions;
        
        /// The errors for GL_INVALID_ENUM
        const std::vector<std::string> &
        invalidEnum() const
        {
            return errorDescriptions.at(GLenum(GL_INVALID_ENUM));
        }
        
        /// The errors for GL_INVALID_VALUE
        const std::vector<std::string> &
        invalidValue() const
        {
            return errorDescriptions.at(GLenum(GL_INVALID_VALUE));
        }
        
        /// The errors for GL_INVALID_OPERATION
        const std::vector<std::string> &
        invalidOperation() const
        {
            return errorDescriptions.at(GLenum(GL_INVALID_OPERATION));
        }
        
        /// The errors for GL_INVALID_FRAMEBUFFER_OPERATION
        const std::vector<std::string> &
        invalidFramebufferOperation() const
        {
            return errorDescriptions.at(GLenum(GL_INVALID_FRAMEBUFFER_OPERATION));
        }
        
        /// The errors for GL_OUT_OF_MEMORY
        const std::vector<std::string> &
        outOfMemory() const
        {
            return errorDescriptions.at(GLenum(GL_OUT_OF_MEMORY));
        }
        
        
        
        /// The concatenated result of all the strings in `invalidEnum`
        std::string
        invalidEnumDescription() const
        {
            return joinStringsWithSeperator(invalidEnum(), " ");
        }
        
        /// The concatenated result of all the strings in `invalidValue`
        std::string
        invalidValueDescription() const {
            return joinStringsWithSeperator(invalidValue(), " ");
        }
        
        /// The concatenated result of all the strings in `invalidOperation`
        std::string
        invalidOperationDescription() const
        {
            return joinStringsWithSeperator(invalidOperation(), " ");
        }
        
        /// The concatenated result of all the strings in `invalidFramebufferOperation`
        std::string
        invalidFramebufferOperationDescription() const
        {
            return joinStringsWithSeperator(invalidFramebufferOperation(), " ");
        }
        
        /// The concatenated result of all the strings in `outOfMemory`
        std::string
        outOfMemoryDescription() const
        {
            return joinStringsWithSeperator(outOfMemory(), " ");
        }
        
        /// Constructs this type with a function and all of its associated error descriptions.
        OpenGLAdditionalErrorInfo(
            const std::string & _functionName,
            const std::string & _url,
            const std::vector<std::string> & invalidEnum,
            const std::vector<std::string> & invalidValue,
            const std::vector<std::string> & invalidOperation,
            const std::vector<std::string> & invalidFramebufferOperation,
            const std::vector<std::string> & outOfMemory,
            const std::vector<std::string> & _errorNotes)
        {
            this->functionName = _functionName;
            this->url = _url;
            this->errorNotes = _errorNotes;
            errorDescriptions[GLenum(GL_INVALID_ENUM)] = invalidEnum;
            errorDescriptions[GLenum(GL_INVALID_VALUE)] = invalidValue;
            errorDescriptions[GLenum(GL_INVALID_OPERATION)] = invalidOperation;
            errorDescriptions[GLenum(GL_INVALID_FRAMEBUFFER_OPERATION)] = invalidFramebufferOperation;
            errorDescriptions[GLenum(GL_OUT_OF_MEMORY)] = outOfMemory;
        }
        
        /// Returns the the list of errors correlate to the enums found in `info`
        std::vector<std::string>
        errorsForEnumInfo(const OpenGLEnumInfo & info) const
        {
            std::vector<std::string> result;
            if (info.invalidEnum()) {
                result.insert(result.end(), invalidEnum().begin(), invalidEnum().end());
            }
            if (info.invalidValue()) {
                result.insert(result.end(), invalidValue().begin(), invalidValue().end());
            }
            if (info.invalidOperation()) {
                result.insert(result.end(), invalidOperation().begin(), invalidOperation().end());
            }
            if (info.invalidFramebufferOperation()) {
                result.insert(result.end(), invalidFramebufferOperation().begin(), invalidFramebufferOperation().end());
            }
            if (info.outOfMemory()) {
                result.insert(result.end(), outOfMemory().begin(), outOfMemory().end());
            }
            
            return result;
        }
        
        /// Returns all of error messages congealed together into a single string.
        std::string
        description() const
        {
            return invalidEnumDescription()
             + " " + invalidValueDescription()
             + " " + invalidOperationDescription()
             + " " + invalidFramebufferOperationDescription()
             + " " + outOfMemoryDescription()
             + " " + joinStringsWithSeperator(errorNotes, " ");
        }
    };

    /// Returns whether an OpenGL error has occurred, and a general error description
    /// for all of the errors found, as well as all of the error enums in an array.
    /// After this function is called the caller is guaranteed that
    /// glGetError() == GL_NO_ERROR (basically this function clears the error state
    /// for OpenGL)
    static OpenGLStateErrorInfo
    GetOpenGLStateErrors()
    {
        std::string toRet = "";
        GLenum error = glGetError();
        std::vector<OpenGLEnumInfo> rawErrors;
        bool foundError = false;
        
        if (error != GLenum(GL_NO_ERROR)) {
            foundError = true;
        }
        
        while (error != GLenum(GL_NO_ERROR)) {
            OpenGLEnumInfo info = OpenGLEnumInfo(error);
            toRet += info.generalErrorDescription();
            rawErrors.push_back(info);
            
            error = glGetError();
        }
        
        OpenGLStateErrorInfo toReturn;
        
        toReturn.encounteredError = foundError;
        toReturn.description = toRet;
        toReturn.rawErrors = rawErrors;
        
        return toReturn;
    }
    
    /// Tries to query the OpenGL website https://www.opengl.org for the
    /// documentation of `function`.
    static OpenGLAdditionalErrorInfo
    RequestOpenGLAPIErrorInfoForFunction(
        const std::string & function)
    {
        std::vector<std::string> invalidEnum, invalidValue, invalidOperation, invalidFramebufferOperation, outOfMemory, errorNotes;
        std::string url;
        
#ifdef GLEE_NETWORKING
        URL_FILE * urlDataStream = NULL;
        int subStringLen = (int) function.length();
        std::string functionGroupName;
        
        while (urlDataStream == NULL && subStringLen > 0) {
            functionGroupName = function.substr(0, (std::string::size_type)subStringLen);
            url = "https://www.opengl.org/sdk/docs/man/html/" + functionGroupName + ".xhtml";
            
            urlDataStream = url_fopen(url.c_str(), "r");
            if (urlDataStream == NULL) {
                subStringLen--;
            }
        }
        
        if (urlDataStream != NULL) {
            std::string urlData = "";
            xmlChar buffer[256] = {'\0'};
            while (url_fread(buffer, sizeof(buffer), sizeof(char), urlDataStream) > 0) {
                for (int i = 0; i < 256; i++) {
                    /// We intentionally skip over all of the non-ASCII characters
                    if (buffer[i] > 0 && buffer[i] < 128) {
                        urlData.push_back((char)buffer[i]);
                    }
                }
            }
            htmlDocPtr doc = htmlReadDoc((xmlChar*)urlData.c_str(), NULL, NULL,
             HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
            xmlNodePtr root = xmlDocGetRootElement(doc);
            std::vector<xmlNodePtr> errorSearch
             = findXMLNodeChildrenForXPath("//div[@id='errors']/p", root);
            for (std::vector<xmlNodePtr>::iterator itr = errorSearch.begin();
                itr != errorSearch.end(); itr++) {
                
                std::string content = unannotedXMLNodeContent(*itr);
                if (content.find("GL_INVALID_ENUM") != std::string::npos) {
                    invalidEnum.push_back(content);
                }
                else if (content.find("GL_INVALID_VALUE") != std::string::npos) {
                    invalidValue.push_back(content);
                }
                else if (content.find("GL_INVALID_OPERATION") != std::string::npos) {
                    invalidOperation.push_back(content);
                }
                else if (content.find("GL_INVALID_FRAMEBUFFER_OPERATION") != std::string::npos) {
                    invalidFramebufferOperation.push_back(content);
                }
                else if (content.find("GL_OUT_OF_MEMORY") != std::string::npos) {
                    outOfMemory.push_back(content);
                }
                else {
                    errorNotes.push_back(content);
                }
            }
            
            xmlFreeDoc(doc);
            url_fclose(urlDataStream);
        }
        else {
            std::cerr << "{" << __FILE__ << ":" << __LINE__
             << " (" << __FUNCTION__ << ")} "
             << "Could not find documentation for function " << function
             << std::endl;
        }
#endif // GLEE_NETWORKING
        
        return OpenGLAdditionalErrorInfo(function, url, invalidEnum,
         invalidValue, invalidOperation, invalidFramebufferOperation,
         outOfMemory, errorNotes);
    }
    
#endif // GLEE_OVERWRITE_GL_FUNCTIONS


////////////////////////////////////////////////////////////////////////////////
///
/// 4. Networking
///

#if defined(GLEE_NETWORKING)
    
    ///
    enum fcurl_type_e {
        CFTYPE_NONE=0,
        CFTYPE_FILE=1,
        CFTYPE_CURL=2
    };

    ///
    struct fcurl_data
    {
        enum fcurl_type_e type;     /* type of handle */
        union {
            CURL *curl;
            FILE *file;
        } handle;                   /* handle */
        
        char *buffer;               /* buffer to store cached data*/
        size_t buffer_len;          /* currently allocated buffers length */
        size_t buffer_pos;          /* end of data in buffer*/
        int still_running;          /* Is background url fetch still in progress */
    };

    ///
    typedef struct fcurl_data URL_FILE;
    
    /// Adapted from "node.cc" in the C++ libxml wrapper found here:
    ///     http://ftp.gnome.org/pub/GNOME/sources/libxml++/3.0/
    static std::vector<xmlNodePtr>
    findXMLNodeChildrenForXPath(
        const std::string & xpath,
        xmlNode * node)
    {
        
        std::vector<xmlNodePtr> nodes;
        xmlXPathContextPtr ctxt = xmlXPathNewContext(node->doc);
        assert(ctxt != NULL);
        ctxt->node = node;
        
        xmlXPathObjectPtr result = xmlXPathEval((const xmlChar*)xpath.c_str(), ctxt);
        assert(result != NULL); // Bad path given
        assert(result->type == XPATH_NODESET); // Only nodeset result types are supported.
        
        xmlNodeSetPtr nodeset = result->nodesetval;
        if (nodeset && !xmlXPathNodeSetIsEmpty(nodeset)) {
            const int count = xmlXPathNodeSetGetLength(nodeset);
            nodes.reserve((std::vector<xmlNodePtr>::size_type)count);
            for (int i = 0; i != count; ++i) {
                xmlNodePtr cnode = xmlXPathNodeSetItem(nodeset, i);
                
                if (!cnode) {
                    std::cerr << "{" << __FILE__ << ":" << __LINE__
                     << " (" << __FUNCTION__ << ")} "
                     << "The xmlNode found was null???" << std::endl;
                    continue;
                }
                
                if (cnode->type == XML_NAMESPACE_DECL) {
                    continue;
                }
                nodes.push_back(cnode);
            }
        }
        
        xmlXPathFreeObject(result);
        xmlXPathFreeContext(ctxt);
        return nodes;
    }

    ///
    static std::string
    unannotedXMLNodeContent(xmlNode * node)
    {
        std::string result = "";
        xmlChar * encodedString = node->content; ///xmlNodeListGetString(node->doc, node, true);
        if (encodedString != NULL) {
            xmlChar * content = encodedString;
            while (*content != '\0') {
                xmlChar val = *content;
                if (val > 0 && val < 128 && val != '\n' && val != '\t' && !(val == ' ' && content[1] == ' ')) {
                    result += char(val);
                }
                content++;
            }
        }
        xmlNodePtr child = node->children;
        while (child != NULL) {
            result += unannotedXMLNodeContent(child);
            child = child->next;
        }
        
        return result;
    }

    ///
    static CURLM **
    multi_handle_addr()
    {
        static CURLM * __multi_handle = NULL;
        return &__multi_handle;
    }
    
    ///
    static CURLM *
    multi_handle()
    {
        return *multi_handle_addr();
    }

    /// curl calls this routine to get more data
    static size_t
    write_callback(
        char *buffer,
        size_t size,
        size_t nitems,
        void *userp)
    {
        char *newbuff;
        size_t rembuff;
        
        URL_FILE *url = (URL_FILE *)userp;
        size *= nitems;
        
        rembuff=url->buffer_len - url->buffer_pos; /* remaining space in buffer */
        
        if(size > rembuff) {
            /* not enough space in buffer */
            newbuff = (char *) realloc(url->buffer, url->buffer_len + (size - rembuff));
            if(newbuff==NULL) {
                fprintf(stderr, "callback buffer grow failed\n");
                size=rembuff;
            }
            else {
                /* realloc succeeded increase buffer size*/
                url->buffer_len+=size - rembuff;
                url->buffer=newbuff;
            }
        }
        
        memcpy(&url->buffer[url->buffer_pos], buffer, size);
        url->buffer_pos += size;
        
        return size;
    }

    /// use to attempt to fill the read buffer up to requested number of bytes
    static int
    fill_buffer(
        URL_FILE *file,
        size_t want)
    {
        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        struct timeval timeout;
        int rc;
        CURLMcode mc; /* curl_multi_fdset() return code */
        
        /* only attempt to fill buffer if transactions still running and buffer
         * doesn't exceed required size already
         */
        if((!file->still_running) || (file->buffer_pos > want))
            return 0;
        
        /* attempt to fill buffer */
        do {
            int maxfd = -1;
            long curl_timeo = -1;
            
            FD_ZERO(&fdread);
            FD_ZERO(&fdwrite);
            FD_ZERO(&fdexcep);
            
            /* set a suitable timeout to fail on */
            timeout.tv_sec = 60; /* 1 minute */
            timeout.tv_usec = 0;
            
            curl_multi_timeout(multi_handle(), &curl_timeo);
            if(curl_timeo >= 0) {
                timeout.tv_sec = curl_timeo / 1000;
                if(timeout.tv_sec > 1)
                    timeout.tv_sec = 1;
                else
                    timeout.tv_usec = (curl_timeo % 1000) * 1000;
            }
            
            /* get file descriptors from the transfers */
            mc = curl_multi_fdset(multi_handle(), &fdread, &fdwrite, &fdexcep, &maxfd);
            
            if(mc != CURLM_OK) {
                fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
                break;
            }
            
            /* On success the value of maxfd is guaranteed to be >= -1. We call
             select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
             no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
             to sleep 100ms, which is the minimum suggested value in the
             curl_multi_fdset() doc. */
            
            if(maxfd == -1) {
    #ifdef _WIN32
                Sleep(100);
                rc = 0;
    #else
                /* Portable sleep for platforms other than Windows. */
                struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
                rc = select(0, NULL, NULL, NULL, &wait);
    #endif
            }
            else {
                /* Note that on some platforms 'timeout' may be modified by select().
                 If you need access to the original value save a copy beforehand. */
                rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
            }
            
            switch(rc) {
                case -1:
                    /* select error */
                    break;
                    
                case 0:
                default:
                    /* timeout or readable/writable sockets */
                    curl_multi_perform(multi_handle(), &file->still_running);
                    break;
            }
        } while(file->still_running && (file->buffer_pos < want));
        return 1;
    }

    /// use to remove want bytes from the front of a files buffer
    static int
    use_buffer(
        URL_FILE *file,
        size_t want)
    {
        /* sort out buffer */
        if((file->buffer_pos - want) <=0) {
            /* ditch buffer - write will recreate */
            free(file->buffer);
            file->buffer=NULL;
            file->buffer_pos=0;
            file->buffer_len=0;
        }
        else {
            /* move rest down make it available for later */
            memmove(file->buffer,
                    &file->buffer[want],
                    (file->buffer_pos - want));
            
            file->buffer_pos -= want;
        }
        return 0;
    }

    static URL_FILE *
    url_fopen(
        const char *url,
        const char *operation)
    {
        /* this code could check for URLs or types in the 'url' and
         basically use the real fopen() for standard files */
        
        URL_FILE *file;
        (void)operation;
        
        file = (URL_FILE *) malloc(sizeof(URL_FILE));
        if(!file)
            return NULL;
        
        memset(file, 0, sizeof(URL_FILE));
        
        if((file->handle.file=fopen(url, operation)))
            file->type = CFTYPE_FILE; /* marked as URL */
        
        else {
            file->type = CFTYPE_CURL; /* marked as URL */
            file->handle.curl = curl_easy_init();
            
            curl_easy_setopt(file->handle.curl, CURLOPT_URL, url);
            curl_easy_setopt(file->handle.curl, CURLOPT_WRITEDATA, file);
            curl_easy_setopt(file->handle.curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(file->handle.curl, CURLOPT_WRITEFUNCTION, write_callback);
            
            if(!multi_handle())
                *multi_handle_addr() = curl_multi_init();
            
            curl_multi_add_handle(multi_handle(), file->handle.curl);
            
            /* lets start the fetch */
            curl_multi_perform(multi_handle(), &file->still_running);
            
            if((file->buffer_pos == 0) && (!file->still_running)) {
                /* if still_running is 0 now, we should return NULL */
                
                /* make sure the easy handle is not in the multi handle anymore */
                curl_multi_remove_handle(multi_handle(), file->handle.curl);
                
                /* cleanup */
                curl_easy_cleanup(file->handle.curl);
                
                free(file);
                
                file = NULL;
            }
        }
        return file;
    }

    ///
    static int
    url_fclose(URL_FILE *file)
    {
        int ret=0;/* default is good return */
        
        switch(file->type) {
            case CFTYPE_FILE:
                ret=fclose(file->handle.file); /* passthrough */
                break;
                
            case CFTYPE_CURL:
                /* make sure the easy handle is not in the multi handle anymore */
                curl_multi_remove_handle(multi_handle(), file->handle.curl);
                
                /* cleanup */
                curl_easy_cleanup(file->handle.curl);
                break;
                
            default: /* unknown or supported type - oh dear */
                ret=EOF;
                errno=EBADF;
                break;
        }
        
        free(file->buffer);/* free any allocated buffer space */
        free(file);
        
        return ret;
    }

    ///
    static size_t
    url_fread(
        void *ptr,
        size_t size,
        size_t nmemb,
        URL_FILE *file)
    {
        size_t want;
        
        switch(file->type) {
            case CFTYPE_FILE:
                want = fread(ptr, size, nmemb, file->handle.file);
                break;
                
            case CFTYPE_CURL:
                want = nmemb * size;
                
                fill_buffer(file, want);
                
                /* check if theres data in the buffer - if not fill_buffer()
                 * either errored or EOF */
                if(!file->buffer_pos)
                    return 0;
                
                /* ensure only available data is considered */
                if(file->buffer_pos < want)
                    want = file->buffer_pos;
                
                /* xfer data to caller */
                memcpy(ptr, file->buffer, want);
                
                use_buffer(file, want);
                
                want = want / size;     /* number of items */
                break;
                
            default: /* unknown or supported type - oh dear */
                want = 0;
                errno = EBADF;
                break;
                
        }
        return want;
    }
    
#endif // defined(GLEE_NETWORKING)

////////////////////////////////////////////////////////////////////////////////
///
/// 5. OpenGL Redifinitions
///
}; // struct glee_api

////////////////////////////////////////////////////////////////////////////////
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

////////////////////////////////////////////////////////////////////////////////
///  (Automatically generated by gen_glee.py)
///  The following removes any glew #define's over our OpenGL functions

#if defined(GLEE_OVERWRITE_GL_FUNCTIONS)
#if defined(__GLEW_H__)

#undef glActiveShaderProgram
#undef glActiveTexture
#undef glAttachShader
#undef glBeginConditionalRender
#undef glBeginQuery
#undef glBeginQueryIndexed
#undef glBeginTransformFeedback
#undef glBindAttribLocation
#undef glBindBuffer
#undef glBindBufferBase
#undef glBindBufferRange
#undef glBindFragDataLocation
#undef glBindFragDataLocationIndexed
#undef glBindFramebuffer
#undef glBindProgramPipeline
#undef glBindRenderbuffer
#undef glBindSampler
#undef glBindTransformFeedback
#undef glBindVertexArray
#undef glBlendColor
#undef glBlendEquation
#undef glBlendEquationSeparate
#undef glBlendEquationSeparatei
#undef glBlendEquationi
#undef glBlendFuncSeparate
#undef glBlendFuncSeparatei
#undef glBlendFunci
#undef glBlitFramebuffer
#undef glBufferData
#undef glBufferSubData
#undef glCheckFramebufferStatus
#undef glClampColor
#undef glClearBufferfi
#undef glClearBufferfv
#undef glClearBufferiv
#undef glClearBufferuiv
#undef glClearDepthf
#undef glClientWaitSync
#undef glColorMaski
#undef glCompileShader
#undef glCompressedTexImage1D
#undef glCompressedTexImage2D
#undef glCompressedTexImage3D
#undef glCompressedTexSubImage1D
#undef glCompressedTexSubImage2D
#undef glCompressedTexSubImage3D
#undef glCopyBufferSubData
#undef glCopyTexSubImage3D
#undef glCreateProgram
#undef glCreateShader
#undef glCreateShaderProgramv
#undef glDeleteBuffers
#undef glDeleteFramebuffers
#undef glDeleteProgram
#undef glDeleteProgramPipelines
#undef glDeleteQueries
#undef glDeleteRenderbuffers
#undef glDeleteSamplers
#undef glDeleteShader
#undef glDeleteSync
#undef glDeleteTransformFeedbacks
#undef glDeleteVertexArrays
#undef glDepthRangeArrayv
#undef glDepthRangeIndexed
#undef glDepthRangef
#undef glDetachShader
#undef glDisableVertexAttribArray
#undef glDisablei
#undef glDrawArraysIndirect
#undef glDrawArraysInstanced
#undef glDrawBuffers
#undef glDrawElementsBaseVertex
#undef glDrawElementsIndirect
#undef glDrawElementsInstanced
#undef glDrawElementsInstancedBaseVertex
#undef glDrawRangeElements
#undef glDrawRangeElementsBaseVertex
#undef glDrawTransformFeedback
#undef glDrawTransformFeedbackStream
#undef glEnableVertexAttribArray
#undef glEnablei
#undef glEndConditionalRender
#undef glEndQuery
#undef glEndQueryIndexed
#undef glEndTransformFeedback
#undef glFenceSync
#undef glFlushMappedBufferRange
#undef glFramebufferRenderbuffer
#undef glFramebufferTexture
#undef glFramebufferTexture1D
#undef glFramebufferTexture2D
#undef glFramebufferTexture3D
#undef glFramebufferTextureLayer
#undef glGenBuffers
#undef glGenFramebuffers
#undef glGenProgramPipelines
#undef glGenQueries
#undef glGenRenderbuffers
#undef glGenSamplers
#undef glGenTransformFeedbacks
#undef glGenVertexArrays
#undef glGenerateMipmap
#undef glGetActiveAttrib
#undef glGetActiveSubroutineName
#undef glGetActiveSubroutineUniformName
#undef glGetActiveSubroutineUniformiv
#undef glGetActiveUniform
#undef glGetActiveUniformBlockName
#undef glGetActiveUniformBlockiv
#undef glGetActiveUniformName
#undef glGetActiveUniformsiv
#undef glGetAttachedShaders
#undef glGetAttribLocation
#undef glGetBooleani_v
#undef glGetBufferParameteri64v
#undef glGetBufferParameteriv
#undef glGetBufferPointerv
#undef glGetBufferSubData
#undef glGetCompressedTexImage
#undef glGetDoublei_v
#undef glGetFloati_v
#undef glGetFragDataIndex
#undef glGetFragDataLocation
#undef glGetFramebufferAttachmentParameteriv
#undef glGetInteger64i_v
#undef glGetInteger64v
#undef glGetIntegeri_v
#undef glGetMultisamplefv
#undef glGetProgramBinary
#undef glGetProgramInfoLog
#undef glGetProgramPipelineInfoLog
#undef glGetProgramPipelineiv
#undef glGetProgramStageiv
#undef glGetProgramiv
#undef glGetQueryIndexediv
#undef glGetQueryObjecti64v
#undef glGetQueryObjectiv
#undef glGetQueryObjectui64v
#undef glGetQueryObjectuiv
#undef glGetQueryiv
#undef glGetRenderbufferParameteriv
#undef glGetSamplerParameterIiv
#undef glGetSamplerParameterIuiv
#undef glGetSamplerParameterfv
#undef glGetSamplerParameteriv
#undef glGetShaderInfoLog
#undef glGetShaderPrecisionFormat
#undef glGetShaderSource
#undef glGetShaderiv
#undef glGetStringi
#undef glGetSubroutineIndex
#undef glGetSubroutineUniformLocation
#undef glGetSynciv
#undef glGetTexParameterIiv
#undef glGetTexParameterIuiv
#undef glGetTransformFeedbackVarying
#undef glGetUniformBlockIndex
#undef glGetUniformIndices
#undef glGetUniformLocation
#undef glGetUniformSubroutineuiv
#undef glGetUniformdv
#undef glGetUniformfv
#undef glGetUniformiv
#undef glGetUniformuiv
#undef glGetVertexAttribIiv
#undef glGetVertexAttribIuiv
#undef glGetVertexAttribLdv
#undef glGetVertexAttribPointerv
#undef glGetVertexAttribdv
#undef glGetVertexAttribfv
#undef glGetVertexAttribiv
#undef glIsBuffer
#undef glIsEnabledi
#undef glIsFramebuffer
#undef glIsProgram
#undef glIsProgramPipeline
#undef glIsQuery
#undef glIsRenderbuffer
#undef glIsSampler
#undef glIsShader
#undef glIsSync
#undef glIsTransformFeedback
#undef glIsVertexArray
#undef glLinkProgram
#undef glMapBuffer
#undef glMapBufferRange
#undef glMinSampleShading
#undef glMultiDrawArrays
#undef glMultiDrawElements
#undef glMultiDrawElementsBaseVertex
#undef glPatchParameterfv
#undef glPatchParameteri
#undef glPauseTransformFeedback
#undef glPointParameterf
#undef glPointParameterfv
#undef glPointParameteri
#undef glPointParameteriv
#undef glPrimitiveRestartIndex
#undef glProgramBinary
#undef glProgramParameteri
#undef glProgramUniform1d
#undef glProgramUniform1dv
#undef glProgramUniform1f
#undef glProgramUniform1fv
#undef glProgramUniform1i
#undef glProgramUniform1iv
#undef glProgramUniform1ui
#undef glProgramUniform1uiv
#undef glProgramUniform2d
#undef glProgramUniform2dv
#undef glProgramUniform2f
#undef glProgramUniform2fv
#undef glProgramUniform2i
#undef glProgramUniform2iv
#undef glProgramUniform2ui
#undef glProgramUniform2uiv
#undef glProgramUniform3d
#undef glProgramUniform3dv
#undef glProgramUniform3f
#undef glProgramUniform3fv
#undef glProgramUniform3i
#undef glProgramUniform3iv
#undef glProgramUniform3ui
#undef glProgramUniform3uiv
#undef glProgramUniform4d
#undef glProgramUniform4dv
#undef glProgramUniform4f
#undef glProgramUniform4fv
#undef glProgramUniform4i
#undef glProgramUniform4iv
#undef glProgramUniform4ui
#undef glProgramUniform4uiv
#undef glProgramUniformMatrix2dv
#undef glProgramUniformMatrix2fv
#undef glProgramUniformMatrix2x3dv
#undef glProgramUniformMatrix2x3fv
#undef glProgramUniformMatrix2x4dv
#undef glProgramUniformMatrix2x4fv
#undef glProgramUniformMatrix3dv
#undef glProgramUniformMatrix3fv
#undef glProgramUniformMatrix3x2dv
#undef glProgramUniformMatrix3x2fv
#undef glProgramUniformMatrix3x4dv
#undef glProgramUniformMatrix3x4fv
#undef glProgramUniformMatrix4dv
#undef glProgramUniformMatrix4fv
#undef glProgramUniformMatrix4x2dv
#undef glProgramUniformMatrix4x2fv
#undef glProgramUniformMatrix4x3dv
#undef glProgramUniformMatrix4x3fv
#undef glProvokingVertex
#undef glQueryCounter
#undef glReleaseShaderCompiler
#undef glRenderbufferStorage
#undef glRenderbufferStorageMultisample
#undef glResumeTransformFeedback
#undef glSampleCoverage
#undef glSampleMaski
#undef glSamplerParameterIiv
#undef glSamplerParameterIuiv
#undef glSamplerParameterf
#undef glSamplerParameterfv
#undef glSamplerParameteri
#undef glSamplerParameteriv
#undef glScissorArrayv
#undef glScissorIndexed
#undef glScissorIndexedv
#undef glShaderBinary
#undef glShaderSource
#undef glStencilFuncSeparate
#undef glStencilMaskSeparate
#undef glStencilOpSeparate
#undef glTexBuffer
#undef glTexImage2DMultisample
#undef glTexImage3D
#undef glTexImage3DMultisample
#undef glTexParameterIiv
#undef glTexParameterIuiv
#undef glTexSubImage3D
#undef glTransformFeedbackVaryings
#undef glUniform1d
#undef glUniform1dv
#undef glUniform1f
#undef glUniform1fv
#undef glUniform1i
#undef glUniform1iv
#undef glUniform1ui
#undef glUniform1uiv
#undef glUniform2d
#undef glUniform2dv
#undef glUniform2f
#undef glUniform2fv
#undef glUniform2i
#undef glUniform2iv
#undef glUniform2ui
#undef glUniform2uiv
#undef glUniform3d
#undef glUniform3dv
#undef glUniform3f
#undef glUniform3fv
#undef glUniform3i
#undef glUniform3iv
#undef glUniform3ui
#undef glUniform3uiv
#undef glUniform4d
#undef glUniform4dv
#undef glUniform4f
#undef glUniform4fv
#undef glUniform4i
#undef glUniform4iv
#undef glUniform4ui
#undef glUniform4uiv
#undef glUniformBlockBinding
#undef glUniformMatrix2dv
#undef glUniformMatrix2fv
#undef glUniformMatrix2x3dv
#undef glUniformMatrix2x3fv
#undef glUniformMatrix2x4dv
#undef glUniformMatrix2x4fv
#undef glUniformMatrix3dv
#undef glUniformMatrix3fv
#undef glUniformMatrix3x2dv
#undef glUniformMatrix3x2fv
#undef glUniformMatrix3x4dv
#undef glUniformMatrix3x4fv
#undef glUniformMatrix4dv
#undef glUniformMatrix4fv
#undef glUniformMatrix4x2dv
#undef glUniformMatrix4x2fv
#undef glUniformMatrix4x3dv
#undef glUniformMatrix4x3fv
#undef glUniformSubroutinesuiv
#undef glUnmapBuffer
#undef glUseProgram
#undef glUseProgramStages
#undef glValidateProgram
#undef glValidateProgramPipeline
#undef glVertexAttrib1d
#undef glVertexAttrib1dv
#undef glVertexAttrib1f
#undef glVertexAttrib1fv
#undef glVertexAttrib1s
#undef glVertexAttrib1sv
#undef glVertexAttrib2d
#undef glVertexAttrib2dv
#undef glVertexAttrib2f
#undef glVertexAttrib2fv
#undef glVertexAttrib2s
#undef glVertexAttrib2sv
#undef glVertexAttrib3d
#undef glVertexAttrib3dv
#undef glVertexAttrib3f
#undef glVertexAttrib3fv
#undef glVertexAttrib3s
#undef glVertexAttrib3sv
#undef glVertexAttrib4Nbv
#undef glVertexAttrib4Niv
#undef glVertexAttrib4Nsv
#undef glVertexAttrib4Nub
#undef glVertexAttrib4Nubv
#undef glVertexAttrib4Nuiv
#undef glVertexAttrib4Nusv
#undef glVertexAttrib4bv
#undef glVertexAttrib4d
#undef glVertexAttrib4dv
#undef glVertexAttrib4f
#undef glVertexAttrib4fv
#undef glVertexAttrib4iv
#undef glVertexAttrib4s
#undef glVertexAttrib4sv
#undef glVertexAttrib4ubv
#undef glVertexAttrib4uiv
#undef glVertexAttrib4usv
#undef glVertexAttribDivisor
#undef glVertexAttribI1i
#undef glVertexAttribI1iv
#undef glVertexAttribI1ui
#undef glVertexAttribI1uiv
#undef glVertexAttribI2i
#undef glVertexAttribI2iv
#undef glVertexAttribI2ui
#undef glVertexAttribI2uiv
#undef glVertexAttribI3i
#undef glVertexAttribI3iv
#undef glVertexAttribI3ui
#undef glVertexAttribI3uiv
#undef glVertexAttribI4bv
#undef glVertexAttribI4i
#undef glVertexAttribI4iv
#undef glVertexAttribI4sv
#undef glVertexAttribI4ubv
#undef glVertexAttribI4ui
#undef glVertexAttribI4uiv
#undef glVertexAttribI4usv
#undef glVertexAttribIPointer
#undef glVertexAttribL1d
#undef glVertexAttribL1dv
#undef glVertexAttribL2d
#undef glVertexAttribL2dv
#undef glVertexAttribL3d
#undef glVertexAttribL3dv
#undef glVertexAttribL4d
#undef glVertexAttribL4dv
#undef glVertexAttribLPointer
#undef glVertexAttribP1ui
#undef glVertexAttribP1uiv
#undef glVertexAttribP2ui
#undef glVertexAttribP2uiv
#undef glVertexAttribP3ui
#undef glVertexAttribP3uiv
#undef glVertexAttribP4ui
#undef glVertexAttribP4uiv
#undef glVertexAttribPointer
#undef glViewportArrayv
#undef glViewportIndexedf
#undef glViewportIndexedfv
#undef glWaitSync

#endif // defined(__GLEW_H_)
#endif // defined(GLEE_OVERWRITE_GL_FUNCTIONS)

////////////////////////////////////////////////////////////////////////////////
///  (Automatically generated by gen_glee.py)
///  The following defines the 'real name' for each OpenGL function.

#if defined(GLEE_OVERWRITE_GL_FUNCTIONS)

#define glAccum_RealName glAccum
#define glAlphaFunc_RealName glAlphaFunc
#define glAreTexturesResident_RealName glAreTexturesResident
#define glArrayElement_RealName glArrayElement
#define glBegin_RealName glBegin
#define glBindTexture_RealName glBindTexture
#define glBitmap_RealName glBitmap
#define glBlendFunc_RealName glBlendFunc
#define glCallList_RealName glCallList
#define glCallLists_RealName glCallLists
#define glClear_RealName glClear
#define glClearAccum_RealName glClearAccum
#define glClearColor_RealName glClearColor
#define glClearDepth_RealName glClearDepth
#define glClearIndex_RealName glClearIndex
#define glClearStencil_RealName glClearStencil
#define glClipPlane_RealName glClipPlane
#define glColor3b_RealName glColor3b
#define glColor3bv_RealName glColor3bv
#define glColor3d_RealName glColor3d
#define glColor3dv_RealName glColor3dv
#define glColor3f_RealName glColor3f
#define glColor3fv_RealName glColor3fv
#define glColor3i_RealName glColor3i
#define glColor3iv_RealName glColor3iv
#define glColor3s_RealName glColor3s
#define glColor3sv_RealName glColor3sv
#define glColor3ub_RealName glColor3ub
#define glColor3ubv_RealName glColor3ubv
#define glColor3ui_RealName glColor3ui
#define glColor3uiv_RealName glColor3uiv
#define glColor3us_RealName glColor3us
#define glColor3usv_RealName glColor3usv
#define glColor4b_RealName glColor4b
#define glColor4bv_RealName glColor4bv
#define glColor4d_RealName glColor4d
#define glColor4dv_RealName glColor4dv
#define glColor4f_RealName glColor4f
#define glColor4fv_RealName glColor4fv
#define glColor4i_RealName glColor4i
#define glColor4iv_RealName glColor4iv
#define glColor4s_RealName glColor4s
#define glColor4sv_RealName glColor4sv
#define glColor4ub_RealName glColor4ub
#define glColor4ubv_RealName glColor4ubv
#define glColor4ui_RealName glColor4ui
#define glColor4uiv_RealName glColor4uiv
#define glColor4us_RealName glColor4us
#define glColor4usv_RealName glColor4usv
#define glColorMask_RealName glColorMask
#define glColorMaterial_RealName glColorMaterial
#define glColorPointer_RealName glColorPointer
#define glCopyPixels_RealName glCopyPixels
#define glCopyTexImage1D_RealName glCopyTexImage1D
#define glCopyTexImage2D_RealName glCopyTexImage2D
#define glCopyTexSubImage1D_RealName glCopyTexSubImage1D
#define glCopyTexSubImage2D_RealName glCopyTexSubImage2D
#define glCullFace_RealName glCullFace
#define glDeleteLists_RealName glDeleteLists
#define glDeleteTextures_RealName glDeleteTextures
#define glDepthFunc_RealName glDepthFunc
#define glDepthMask_RealName glDepthMask
#define glDepthRange_RealName glDepthRange
#define glDisable_RealName glDisable
#define glDisableClientState_RealName glDisableClientState
#define glDrawArrays_RealName glDrawArrays
#define glDrawBuffer_RealName glDrawBuffer
#define glDrawElements_RealName glDrawElements
#define glDrawPixels_RealName glDrawPixels
#define glEdgeFlag_RealName glEdgeFlag
#define glEdgeFlagPointer_RealName glEdgeFlagPointer
#define glEdgeFlagv_RealName glEdgeFlagv
#define glEnable_RealName glEnable
#define glEnableClientState_RealName glEnableClientState
#define glEnd_RealName glEnd
#define glEndList_RealName glEndList
#define glEvalCoord1d_RealName glEvalCoord1d
#define glEvalCoord1dv_RealName glEvalCoord1dv
#define glEvalCoord1f_RealName glEvalCoord1f
#define glEvalCoord1fv_RealName glEvalCoord1fv
#define glEvalCoord2d_RealName glEvalCoord2d
#define glEvalCoord2dv_RealName glEvalCoord2dv
#define glEvalCoord2f_RealName glEvalCoord2f
#define glEvalCoord2fv_RealName glEvalCoord2fv
#define glEvalMesh1_RealName glEvalMesh1
#define glEvalMesh2_RealName glEvalMesh2
#define glEvalPoint1_RealName glEvalPoint1
#define glEvalPoint2_RealName glEvalPoint2
#define glFeedbackBuffer_RealName glFeedbackBuffer
#define glFinish_RealName glFinish
#define glFlush_RealName glFlush
#define glFogf_RealName glFogf
#define glFogfv_RealName glFogfv
#define glFogi_RealName glFogi
#define glFogiv_RealName glFogiv
#define glFrontFace_RealName glFrontFace
#define glFrustum_RealName glFrustum
#define glGenLists_RealName glGenLists
#define glGenTextures_RealName glGenTextures
#define glGetBooleanv_RealName glGetBooleanv
#define glGetClipPlane_RealName glGetClipPlane
#define glGetDoublev_RealName glGetDoublev
#define glGetError_RealName glGetError
#define glGetFloatv_RealName glGetFloatv
#define glGetIntegerv_RealName glGetIntegerv
#define glGetLightfv_RealName glGetLightfv
#define glGetLightiv_RealName glGetLightiv
#define glGetMapdv_RealName glGetMapdv
#define glGetMapfv_RealName glGetMapfv
#define glGetMapiv_RealName glGetMapiv
#define glGetMaterialfv_RealName glGetMaterialfv
#define glGetMaterialiv_RealName glGetMaterialiv
#define glGetPixelMapfv_RealName glGetPixelMapfv
#define glGetPixelMapuiv_RealName glGetPixelMapuiv
#define glGetPixelMapusv_RealName glGetPixelMapusv
#define glGetPointerv_RealName glGetPointerv
#define glGetPolygonStipple_RealName glGetPolygonStipple
#define glGetString_RealName glGetString
#define glGetTexEnvfv_RealName glGetTexEnvfv
#define glGetTexEnviv_RealName glGetTexEnviv
#define glGetTexGendv_RealName glGetTexGendv
#define glGetTexGenfv_RealName glGetTexGenfv
#define glGetTexGeniv_RealName glGetTexGeniv
#define glGetTexImage_RealName glGetTexImage
#define glGetTexLevelParameterfv_RealName glGetTexLevelParameterfv
#define glGetTexLevelParameteriv_RealName glGetTexLevelParameteriv
#define glGetTexParameterfv_RealName glGetTexParameterfv
#define glGetTexParameteriv_RealName glGetTexParameteriv
#define glHint_RealName glHint
#define glIndexMask_RealName glIndexMask
#define glIndexPointer_RealName glIndexPointer
#define glIndexd_RealName glIndexd
#define glIndexdv_RealName glIndexdv
#define glIndexf_RealName glIndexf
#define glIndexfv_RealName glIndexfv
#define glIndexi_RealName glIndexi
#define glIndexiv_RealName glIndexiv
#define glIndexs_RealName glIndexs
#define glIndexsv_RealName glIndexsv
#define glIndexub_RealName glIndexub
#define glIndexubv_RealName glIndexubv
#define glInitNames_RealName glInitNames
#define glInterleavedArrays_RealName glInterleavedArrays
#define glIsEnabled_RealName glIsEnabled
#define glIsList_RealName glIsList
#define glIsTexture_RealName glIsTexture
#define glLightModelf_RealName glLightModelf
#define glLightModelfv_RealName glLightModelfv
#define glLightModeli_RealName glLightModeli
#define glLightModeliv_RealName glLightModeliv
#define glLightf_RealName glLightf
#define glLightfv_RealName glLightfv
#define glLighti_RealName glLighti
#define glLightiv_RealName glLightiv
#define glLineStipple_RealName glLineStipple
#define glLineWidth_RealName glLineWidth
#define glListBase_RealName glListBase
#define glLoadIdentity_RealName glLoadIdentity
#define glLoadMatrixd_RealName glLoadMatrixd
#define glLoadMatrixf_RealName glLoadMatrixf
#define glLoadName_RealName glLoadName
#define glLogicOp_RealName glLogicOp
#define glMap1d_RealName glMap1d
#define glMap1f_RealName glMap1f
#define glMap2d_RealName glMap2d
#define glMap2f_RealName glMap2f
#define glMapGrid1d_RealName glMapGrid1d
#define glMapGrid1f_RealName glMapGrid1f
#define glMapGrid2d_RealName glMapGrid2d
#define glMapGrid2f_RealName glMapGrid2f
#define glMaterialf_RealName glMaterialf
#define glMaterialfv_RealName glMaterialfv
#define glMateriali_RealName glMateriali
#define glMaterialiv_RealName glMaterialiv
#define glMatrixMode_RealName glMatrixMode
#define glMultMatrixd_RealName glMultMatrixd
#define glMultMatrixf_RealName glMultMatrixf
#define glNewList_RealName glNewList
#define glNormal3b_RealName glNormal3b
#define glNormal3bv_RealName glNormal3bv
#define glNormal3d_RealName glNormal3d
#define glNormal3dv_RealName glNormal3dv
#define glNormal3f_RealName glNormal3f
#define glNormal3fv_RealName glNormal3fv
#define glNormal3i_RealName glNormal3i
#define glNormal3iv_RealName glNormal3iv
#define glNormal3s_RealName glNormal3s
#define glNormal3sv_RealName glNormal3sv
#define glNormalPointer_RealName glNormalPointer
#define glOrtho_RealName glOrtho
#define glPassThrough_RealName glPassThrough
#define glPixelMapfv_RealName glPixelMapfv
#define glPixelMapuiv_RealName glPixelMapuiv
#define glPixelMapusv_RealName glPixelMapusv
#define glPixelStoref_RealName glPixelStoref
#define glPixelStorei_RealName glPixelStorei
#define glPixelTransferf_RealName glPixelTransferf
#define glPixelTransferi_RealName glPixelTransferi
#define glPixelZoom_RealName glPixelZoom
#define glPointSize_RealName glPointSize
#define glPolygonMode_RealName glPolygonMode
#define glPolygonOffset_RealName glPolygonOffset
#define glPolygonStipple_RealName glPolygonStipple
#define glPopAttrib_RealName glPopAttrib
#define glPopClientAttrib_RealName glPopClientAttrib
#define glPopMatrix_RealName glPopMatrix
#define glPopName_RealName glPopName
#define glPrioritizeTextures_RealName glPrioritizeTextures
#define glPushAttrib_RealName glPushAttrib
#define glPushClientAttrib_RealName glPushClientAttrib
#define glPushMatrix_RealName glPushMatrix
#define glPushName_RealName glPushName
#define glRasterPos2d_RealName glRasterPos2d
#define glRasterPos2dv_RealName glRasterPos2dv
#define glRasterPos2f_RealName glRasterPos2f
#define glRasterPos2fv_RealName glRasterPos2fv
#define glRasterPos2i_RealName glRasterPos2i
#define glRasterPos2iv_RealName glRasterPos2iv
#define glRasterPos2s_RealName glRasterPos2s
#define glRasterPos2sv_RealName glRasterPos2sv
#define glRasterPos3d_RealName glRasterPos3d
#define glRasterPos3dv_RealName glRasterPos3dv
#define glRasterPos3f_RealName glRasterPos3f
#define glRasterPos3fv_RealName glRasterPos3fv
#define glRasterPos3i_RealName glRasterPos3i
#define glRasterPos3iv_RealName glRasterPos3iv
#define glRasterPos3s_RealName glRasterPos3s
#define glRasterPos3sv_RealName glRasterPos3sv
#define glRasterPos4d_RealName glRasterPos4d
#define glRasterPos4dv_RealName glRasterPos4dv
#define glRasterPos4f_RealName glRasterPos4f
#define glRasterPos4fv_RealName glRasterPos4fv
#define glRasterPos4i_RealName glRasterPos4i
#define glRasterPos4iv_RealName glRasterPos4iv
#define glRasterPos4s_RealName glRasterPos4s
#define glRasterPos4sv_RealName glRasterPos4sv
#define glReadBuffer_RealName glReadBuffer
#define glReadPixels_RealName glReadPixels
#define glRectd_RealName glRectd
#define glRectdv_RealName glRectdv
#define glRectf_RealName glRectf
#define glRectfv_RealName glRectfv
#define glRecti_RealName glRecti
#define glRectiv_RealName glRectiv
#define glRects_RealName glRects
#define glRectsv_RealName glRectsv
#define glRenderMode_RealName glRenderMode
#define glRotated_RealName glRotated
#define glRotatef_RealName glRotatef
#define glScaled_RealName glScaled
#define glScalef_RealName glScalef
#define glScissor_RealName glScissor
#define glSelectBuffer_RealName glSelectBuffer
#define glShadeModel_RealName glShadeModel
#define glStencilFunc_RealName glStencilFunc
#define glStencilMask_RealName glStencilMask
#define glStencilOp_RealName glStencilOp
#define glTexCoord1d_RealName glTexCoord1d
#define glTexCoord1dv_RealName glTexCoord1dv
#define glTexCoord1f_RealName glTexCoord1f
#define glTexCoord1fv_RealName glTexCoord1fv
#define glTexCoord1i_RealName glTexCoord1i
#define glTexCoord1iv_RealName glTexCoord1iv
#define glTexCoord1s_RealName glTexCoord1s
#define glTexCoord1sv_RealName glTexCoord1sv
#define glTexCoord2d_RealName glTexCoord2d
#define glTexCoord2dv_RealName glTexCoord2dv
#define glTexCoord2f_RealName glTexCoord2f
#define glTexCoord2fv_RealName glTexCoord2fv
#define glTexCoord2i_RealName glTexCoord2i
#define glTexCoord2iv_RealName glTexCoord2iv
#define glTexCoord2s_RealName glTexCoord2s
#define glTexCoord2sv_RealName glTexCoord2sv
#define glTexCoord3d_RealName glTexCoord3d
#define glTexCoord3dv_RealName glTexCoord3dv
#define glTexCoord3f_RealName glTexCoord3f
#define glTexCoord3fv_RealName glTexCoord3fv
#define glTexCoord3i_RealName glTexCoord3i
#define glTexCoord3iv_RealName glTexCoord3iv
#define glTexCoord3s_RealName glTexCoord3s
#define glTexCoord3sv_RealName glTexCoord3sv
#define glTexCoord4d_RealName glTexCoord4d
#define glTexCoord4dv_RealName glTexCoord4dv
#define glTexCoord4f_RealName glTexCoord4f
#define glTexCoord4fv_RealName glTexCoord4fv
#define glTexCoord4i_RealName glTexCoord4i
#define glTexCoord4iv_RealName glTexCoord4iv
#define glTexCoord4s_RealName glTexCoord4s
#define glTexCoord4sv_RealName glTexCoord4sv
#define glTexCoordPointer_RealName glTexCoordPointer
#define glTexEnvf_RealName glTexEnvf
#define glTexEnvfv_RealName glTexEnvfv
#define glTexEnvi_RealName glTexEnvi
#define glTexEnviv_RealName glTexEnviv
#define glTexGend_RealName glTexGend
#define glTexGendv_RealName glTexGendv
#define glTexGenf_RealName glTexGenf
#define glTexGenfv_RealName glTexGenfv
#define glTexGeni_RealName glTexGeni
#define glTexGeniv_RealName glTexGeniv
#define glTexImage1D_RealName glTexImage1D
#define glTexImage2D_RealName glTexImage2D
#define glTexParameterf_RealName glTexParameterf
#define glTexParameterfv_RealName glTexParameterfv
#define glTexParameteri_RealName glTexParameteri
#define glTexParameteriv_RealName glTexParameteriv
#define glTexSubImage1D_RealName glTexSubImage1D
#define glTexSubImage2D_RealName glTexSubImage2D
#define glTranslated_RealName glTranslated
#define glTranslatef_RealName glTranslatef
#define glVertex2d_RealName glVertex2d
#define glVertex2dv_RealName glVertex2dv
#define glVertex2f_RealName glVertex2f
#define glVertex2fv_RealName glVertex2fv
#define glVertex2i_RealName glVertex2i
#define glVertex2iv_RealName glVertex2iv
#define glVertex2s_RealName glVertex2s
#define glVertex2sv_RealName glVertex2sv
#define glVertex3d_RealName glVertex3d
#define glVertex3dv_RealName glVertex3dv
#define glVertex3f_RealName glVertex3f
#define glVertex3fv_RealName glVertex3fv
#define glVertex3i_RealName glVertex3i
#define glVertex3iv_RealName glVertex3iv
#define glVertex3s_RealName glVertex3s
#define glVertex3sv_RealName glVertex3sv
#define glVertex4d_RealName glVertex4d
#define glVertex4dv_RealName glVertex4dv
#define glVertex4f_RealName glVertex4f
#define glVertex4fv_RealName glVertex4fv
#define glVertex4i_RealName glVertex4i
#define glVertex4iv_RealName glVertex4iv
#define glVertex4s_RealName glVertex4s
#define glVertex4sv_RealName glVertex4sv
#define glVertexPointer_RealName glVertexPointer
#define glViewport_RealName glViewport

#if defined(__GLEW_H__)

#define glActiveShaderProgram_RealName GLEW_GET_FUN(__glewActiveShaderProgram)
#define glActiveTexture_RealName GLEW_GET_FUN(__glewActiveTexture)
#define glAttachShader_RealName GLEW_GET_FUN(__glewAttachShader)
#define glBeginConditionalRender_RealName GLEW_GET_FUN(__glewBeginConditionalRender)
#define glBeginQuery_RealName GLEW_GET_FUN(__glewBeginQuery)
#define glBeginQueryIndexed_RealName GLEW_GET_FUN(__glewBeginQueryIndexed)
#define glBeginTransformFeedback_RealName GLEW_GET_FUN(__glewBeginTransformFeedback)
#define glBindAttribLocation_RealName GLEW_GET_FUN(__glewBindAttribLocation)
#define glBindBuffer_RealName GLEW_GET_FUN(__glewBindBuffer)
#define glBindBufferBase_RealName GLEW_GET_FUN(__glewBindBufferBase)
#define glBindBufferRange_RealName GLEW_GET_FUN(__glewBindBufferRange)
#define glBindFragDataLocation_RealName GLEW_GET_FUN(__glewBindFragDataLocation)
#define glBindFragDataLocationIndexed_RealName GLEW_GET_FUN(__glewBindFragDataLocationIndexed)
#define glBindFramebuffer_RealName GLEW_GET_FUN(__glewBindFramebuffer)
#define glBindProgramPipeline_RealName GLEW_GET_FUN(__glewBindProgramPipeline)
#define glBindRenderbuffer_RealName GLEW_GET_FUN(__glewBindRenderbuffer)
#define glBindSampler_RealName GLEW_GET_FUN(__glewBindSampler)
#define glBindTransformFeedback_RealName GLEW_GET_FUN(__glewBindTransformFeedback)
#define glBindVertexArray_RealName GLEW_GET_FUN(__glewBindVertexArray)
#define glBlendColor_RealName GLEW_GET_FUN(__glewBlendColor)
#define glBlendEquation_RealName GLEW_GET_FUN(__glewBlendEquation)
#define glBlendEquationSeparate_RealName GLEW_GET_FUN(__glewBlendEquationSeparate)
#define glBlendEquationSeparatei_RealName GLEW_GET_FUN(__glewBlendEquationSeparatei)
#define glBlendEquationi_RealName GLEW_GET_FUN(__glewBlendEquationi)
#define glBlendFuncSeparate_RealName GLEW_GET_FUN(__glewBlendFuncSeparate)
#define glBlendFuncSeparatei_RealName GLEW_GET_FUN(__glewBlendFuncSeparatei)
#define glBlendFunci_RealName GLEW_GET_FUN(__glewBlendFunci)
#define glBlitFramebuffer_RealName GLEW_GET_FUN(__glewBlitFramebuffer)
#define glBufferData_RealName GLEW_GET_FUN(__glewBufferData)
#define glBufferSubData_RealName GLEW_GET_FUN(__glewBufferSubData)
#define glCheckFramebufferStatus_RealName GLEW_GET_FUN(__glewCheckFramebufferStatus)
#define glClampColor_RealName GLEW_GET_FUN(__glewClampColor)
#define glClearBufferfi_RealName GLEW_GET_FUN(__glewClearBufferfi)
#define glClearBufferfv_RealName GLEW_GET_FUN(__glewClearBufferfv)
#define glClearBufferiv_RealName GLEW_GET_FUN(__glewClearBufferiv)
#define glClearBufferuiv_RealName GLEW_GET_FUN(__glewClearBufferuiv)
#define glClearDepthf_RealName GLEW_GET_FUN(__glewClearDepthf)
#define glClientWaitSync_RealName GLEW_GET_FUN(__glewClientWaitSync)
#define glColorMaski_RealName GLEW_GET_FUN(__glewColorMaski)
#define glCompileShader_RealName GLEW_GET_FUN(__glewCompileShader)
#define glCompressedTexImage1D_RealName GLEW_GET_FUN(__glewCompressedTexImage1D)
#define glCompressedTexImage2D_RealName GLEW_GET_FUN(__glewCompressedTexImage2D)
#define glCompressedTexImage3D_RealName GLEW_GET_FUN(__glewCompressedTexImage3D)
#define glCompressedTexSubImage1D_RealName GLEW_GET_FUN(__glewCompressedTexSubImage1D)
#define glCompressedTexSubImage2D_RealName GLEW_GET_FUN(__glewCompressedTexSubImage2D)
#define glCompressedTexSubImage3D_RealName GLEW_GET_FUN(__glewCompressedTexSubImage3D)
#define glCopyBufferSubData_RealName GLEW_GET_FUN(__glewCopyBufferSubData)
#define glCopyTexSubImage3D_RealName GLEW_GET_FUN(__glewCopyTexSubImage3D)
#define glCreateProgram_RealName GLEW_GET_FUN(__glewCreateProgram)
#define glCreateShader_RealName GLEW_GET_FUN(__glewCreateShader)
#define glCreateShaderProgramv_RealName GLEW_GET_FUN(__glewCreateShaderProgramv)
#define glDeleteBuffers_RealName GLEW_GET_FUN(__glewDeleteBuffers)
#define glDeleteFramebuffers_RealName GLEW_GET_FUN(__glewDeleteFramebuffers)
#define glDeleteProgram_RealName GLEW_GET_FUN(__glewDeleteProgram)
#define glDeleteProgramPipelines_RealName GLEW_GET_FUN(__glewDeleteProgramPipelines)
#define glDeleteQueries_RealName GLEW_GET_FUN(__glewDeleteQueries)
#define glDeleteRenderbuffers_RealName GLEW_GET_FUN(__glewDeleteRenderbuffers)
#define glDeleteSamplers_RealName GLEW_GET_FUN(__glewDeleteSamplers)
#define glDeleteShader_RealName GLEW_GET_FUN(__glewDeleteShader)
#define glDeleteSync_RealName GLEW_GET_FUN(__glewDeleteSync)
#define glDeleteTransformFeedbacks_RealName GLEW_GET_FUN(__glewDeleteTransformFeedbacks)
#define glDeleteVertexArrays_RealName GLEW_GET_FUN(__glewDeleteVertexArrays)
#define glDepthRangeArrayv_RealName GLEW_GET_FUN(__glewDepthRangeArrayv)
#define glDepthRangeIndexed_RealName GLEW_GET_FUN(__glewDepthRangeIndexed)
#define glDepthRangef_RealName GLEW_GET_FUN(__glewDepthRangef)
#define glDetachShader_RealName GLEW_GET_FUN(__glewDetachShader)
#define glDisableVertexAttribArray_RealName GLEW_GET_FUN(__glewDisableVertexAttribArray)
#define glDisablei_RealName GLEW_GET_FUN(__glewDisablei)
#define glDrawArraysIndirect_RealName GLEW_GET_FUN(__glewDrawArraysIndirect)
#define glDrawArraysInstanced_RealName GLEW_GET_FUN(__glewDrawArraysInstanced)
#define glDrawBuffers_RealName GLEW_GET_FUN(__glewDrawBuffers)
#define glDrawElementsBaseVertex_RealName GLEW_GET_FUN(__glewDrawElementsBaseVertex)
#define glDrawElementsIndirect_RealName GLEW_GET_FUN(__glewDrawElementsIndirect)
#define glDrawElementsInstanced_RealName GLEW_GET_FUN(__glewDrawElementsInstanced)
#define glDrawElementsInstancedBaseVertex_RealName GLEW_GET_FUN(__glewDrawElementsInstancedBaseVertex)
#define glDrawRangeElements_RealName GLEW_GET_FUN(__glewDrawRangeElements)
#define glDrawRangeElementsBaseVertex_RealName GLEW_GET_FUN(__glewDrawRangeElementsBaseVertex)
#define glDrawTransformFeedback_RealName GLEW_GET_FUN(__glewDrawTransformFeedback)
#define glDrawTransformFeedbackStream_RealName GLEW_GET_FUN(__glewDrawTransformFeedbackStream)
#define glEnableVertexAttribArray_RealName GLEW_GET_FUN(__glewEnableVertexAttribArray)
#define glEnablei_RealName GLEW_GET_FUN(__glewEnablei)
#define glEndConditionalRender_RealName GLEW_GET_FUN(__glewEndConditionalRender)
#define glEndQuery_RealName GLEW_GET_FUN(__glewEndQuery)
#define glEndQueryIndexed_RealName GLEW_GET_FUN(__glewEndQueryIndexed)
#define glEndTransformFeedback_RealName GLEW_GET_FUN(__glewEndTransformFeedback)
#define glFenceSync_RealName GLEW_GET_FUN(__glewFenceSync)
#define glFlushMappedBufferRange_RealName GLEW_GET_FUN(__glewFlushMappedBufferRange)
#define glFramebufferRenderbuffer_RealName GLEW_GET_FUN(__glewFramebufferRenderbuffer)
#define glFramebufferTexture_RealName GLEW_GET_FUN(__glewFramebufferTexture)
#define glFramebufferTexture1D_RealName GLEW_GET_FUN(__glewFramebufferTexture1D)
#define glFramebufferTexture2D_RealName GLEW_GET_FUN(__glewFramebufferTexture2D)
#define glFramebufferTexture3D_RealName GLEW_GET_FUN(__glewFramebufferTexture3D)
#define glFramebufferTextureLayer_RealName GLEW_GET_FUN(__glewFramebufferTextureLayer)
#define glGenBuffers_RealName GLEW_GET_FUN(__glewGenBuffers)
#define glGenFramebuffers_RealName GLEW_GET_FUN(__glewGenFramebuffers)
#define glGenProgramPipelines_RealName GLEW_GET_FUN(__glewGenProgramPipelines)
#define glGenQueries_RealName GLEW_GET_FUN(__glewGenQueries)
#define glGenRenderbuffers_RealName GLEW_GET_FUN(__glewGenRenderbuffers)
#define glGenSamplers_RealName GLEW_GET_FUN(__glewGenSamplers)
#define glGenTransformFeedbacks_RealName GLEW_GET_FUN(__glewGenTransformFeedbacks)
#define glGenVertexArrays_RealName GLEW_GET_FUN(__glewGenVertexArrays)
#define glGenerateMipmap_RealName GLEW_GET_FUN(__glewGenerateMipmap)
#define glGetActiveAttrib_RealName GLEW_GET_FUN(__glewGetActiveAttrib)
#define glGetActiveSubroutineName_RealName GLEW_GET_FUN(__glewGetActiveSubroutineName)
#define glGetActiveSubroutineUniformName_RealName GLEW_GET_FUN(__glewGetActiveSubroutineUniformName)
#define glGetActiveSubroutineUniformiv_RealName GLEW_GET_FUN(__glewGetActiveSubroutineUniformiv)
#define glGetActiveUniform_RealName GLEW_GET_FUN(__glewGetActiveUniform)
#define glGetActiveUniformBlockName_RealName GLEW_GET_FUN(__glewGetActiveUniformBlockName)
#define glGetActiveUniformBlockiv_RealName GLEW_GET_FUN(__glewGetActiveUniformBlockiv)
#define glGetActiveUniformName_RealName GLEW_GET_FUN(__glewGetActiveUniformName)
#define glGetActiveUniformsiv_RealName GLEW_GET_FUN(__glewGetActiveUniformsiv)
#define glGetAttachedShaders_RealName GLEW_GET_FUN(__glewGetAttachedShaders)
#define glGetAttribLocation_RealName GLEW_GET_FUN(__glewGetAttribLocation)
#define glGetBooleani_v_RealName GLEW_GET_FUN(__glewGetBooleani_v)
#define glGetBufferParameteri64v_RealName GLEW_GET_FUN(__glewGetBufferParameteri64v)
#define glGetBufferParameteriv_RealName GLEW_GET_FUN(__glewGetBufferParameteriv)
#define glGetBufferPointerv_RealName GLEW_GET_FUN(__glewGetBufferPointerv)
#define glGetBufferSubData_RealName GLEW_GET_FUN(__glewGetBufferSubData)
#define glGetCompressedTexImage_RealName GLEW_GET_FUN(__glewGetCompressedTexImage)
#define glGetDoublei_v_RealName GLEW_GET_FUN(__glewGetDoublei_v)
#define glGetFloati_v_RealName GLEW_GET_FUN(__glewGetFloati_v)
#define glGetFragDataIndex_RealName GLEW_GET_FUN(__glewGetFragDataIndex)
#define glGetFragDataLocation_RealName GLEW_GET_FUN(__glewGetFragDataLocation)
#define glGetFramebufferAttachmentParameteriv_RealName GLEW_GET_FUN(__glewGetFramebufferAttachmentParameteriv)
#define glGetInteger64i_v_RealName GLEW_GET_FUN(__glewGetInteger64i_v)
#define glGetInteger64v_RealName GLEW_GET_FUN(__glewGetInteger64v)
#define glGetIntegeri_v_RealName GLEW_GET_FUN(__glewGetIntegeri_v)
#define glGetMultisamplefv_RealName GLEW_GET_FUN(__glewGetMultisamplefv)
#define glGetProgramBinary_RealName GLEW_GET_FUN(__glewGetProgramBinary)
#define glGetProgramInfoLog_RealName GLEW_GET_FUN(__glewGetProgramInfoLog)
#define glGetProgramPipelineInfoLog_RealName GLEW_GET_FUN(__glewGetProgramPipelineInfoLog)
#define glGetProgramPipelineiv_RealName GLEW_GET_FUN(__glewGetProgramPipelineiv)
#define glGetProgramStageiv_RealName GLEW_GET_FUN(__glewGetProgramStageiv)
#define glGetProgramiv_RealName GLEW_GET_FUN(__glewGetProgramiv)
#define glGetQueryIndexediv_RealName GLEW_GET_FUN(__glewGetQueryIndexediv)
#define glGetQueryObjecti64v_RealName GLEW_GET_FUN(__glewGetQueryObjecti64v)
#define glGetQueryObjectiv_RealName GLEW_GET_FUN(__glewGetQueryObjectiv)
#define glGetQueryObjectui64v_RealName GLEW_GET_FUN(__glewGetQueryObjectui64v)
#define glGetQueryObjectuiv_RealName GLEW_GET_FUN(__glewGetQueryObjectuiv)
#define glGetQueryiv_RealName GLEW_GET_FUN(__glewGetQueryiv)
#define glGetRenderbufferParameteriv_RealName GLEW_GET_FUN(__glewGetRenderbufferParameteriv)
#define glGetSamplerParameterIiv_RealName GLEW_GET_FUN(__glewGetSamplerParameterIiv)
#define glGetSamplerParameterIuiv_RealName GLEW_GET_FUN(__glewGetSamplerParameterIuiv)
#define glGetSamplerParameterfv_RealName GLEW_GET_FUN(__glewGetSamplerParameterfv)
#define glGetSamplerParameteriv_RealName GLEW_GET_FUN(__glewGetSamplerParameteriv)
#define glGetShaderInfoLog_RealName GLEW_GET_FUN(__glewGetShaderInfoLog)
#define glGetShaderPrecisionFormat_RealName GLEW_GET_FUN(__glewGetShaderPrecisionFormat)
#define glGetShaderSource_RealName GLEW_GET_FUN(__glewGetShaderSource)
#define glGetShaderiv_RealName GLEW_GET_FUN(__glewGetShaderiv)
#define glGetStringi_RealName GLEW_GET_FUN(__glewGetStringi)
#define glGetSubroutineIndex_RealName GLEW_GET_FUN(__glewGetSubroutineIndex)
#define glGetSubroutineUniformLocation_RealName GLEW_GET_FUN(__glewGetSubroutineUniformLocation)
#define glGetSynciv_RealName GLEW_GET_FUN(__glewGetSynciv)
#define glGetTexParameterIiv_RealName GLEW_GET_FUN(__glewGetTexParameterIiv)
#define glGetTexParameterIuiv_RealName GLEW_GET_FUN(__glewGetTexParameterIuiv)
#define glGetTransformFeedbackVarying_RealName GLEW_GET_FUN(__glewGetTransformFeedbackVarying)
#define glGetUniformBlockIndex_RealName GLEW_GET_FUN(__glewGetUniformBlockIndex)
#define glGetUniformIndices_RealName GLEW_GET_FUN(__glewGetUniformIndices)
#define glGetUniformLocation_RealName GLEW_GET_FUN(__glewGetUniformLocation)
#define glGetUniformSubroutineuiv_RealName GLEW_GET_FUN(__glewGetUniformSubroutineuiv)
#define glGetUniformdv_RealName GLEW_GET_FUN(__glewGetUniformdv)
#define glGetUniformfv_RealName GLEW_GET_FUN(__glewGetUniformfv)
#define glGetUniformiv_RealName GLEW_GET_FUN(__glewGetUniformiv)
#define glGetUniformuiv_RealName GLEW_GET_FUN(__glewGetUniformuiv)
#define glGetVertexAttribIiv_RealName GLEW_GET_FUN(__glewGetVertexAttribIiv)
#define glGetVertexAttribIuiv_RealName GLEW_GET_FUN(__glewGetVertexAttribIuiv)
#define glGetVertexAttribLdv_RealName GLEW_GET_FUN(__glewGetVertexAttribLdv)
#define glGetVertexAttribPointerv_RealName GLEW_GET_FUN(__glewGetVertexAttribPointerv)
#define glGetVertexAttribdv_RealName GLEW_GET_FUN(__glewGetVertexAttribdv)
#define glGetVertexAttribfv_RealName GLEW_GET_FUN(__glewGetVertexAttribfv)
#define glGetVertexAttribiv_RealName GLEW_GET_FUN(__glewGetVertexAttribiv)
#define glIsBuffer_RealName GLEW_GET_FUN(__glewIsBuffer)
#define glIsEnabledi_RealName GLEW_GET_FUN(__glewIsEnabledi)
#define glIsFramebuffer_RealName GLEW_GET_FUN(__glewIsFramebuffer)
#define glIsProgram_RealName GLEW_GET_FUN(__glewIsProgram)
#define glIsProgramPipeline_RealName GLEW_GET_FUN(__glewIsProgramPipeline)
#define glIsQuery_RealName GLEW_GET_FUN(__glewIsQuery)
#define glIsRenderbuffer_RealName GLEW_GET_FUN(__glewIsRenderbuffer)
#define glIsSampler_RealName GLEW_GET_FUN(__glewIsSampler)
#define glIsShader_RealName GLEW_GET_FUN(__glewIsShader)
#define glIsSync_RealName GLEW_GET_FUN(__glewIsSync)
#define glIsTransformFeedback_RealName GLEW_GET_FUN(__glewIsTransformFeedback)
#define glIsVertexArray_RealName GLEW_GET_FUN(__glewIsVertexArray)
#define glLinkProgram_RealName GLEW_GET_FUN(__glewLinkProgram)
#define glMapBuffer_RealName GLEW_GET_FUN(__glewMapBuffer)
#define glMapBufferRange_RealName GLEW_GET_FUN(__glewMapBufferRange)
#define glMinSampleShading_RealName GLEW_GET_FUN(__glewMinSampleShading)
#define glMultiDrawArrays_RealName GLEW_GET_FUN(__glewMultiDrawArrays)
#define glMultiDrawElements_RealName GLEW_GET_FUN(__glewMultiDrawElements)
#define glMultiDrawElementsBaseVertex_RealName GLEW_GET_FUN(__glewMultiDrawElementsBaseVertex)
#define glPatchParameterfv_RealName GLEW_GET_FUN(__glewPatchParameterfv)
#define glPatchParameteri_RealName GLEW_GET_FUN(__glewPatchParameteri)
#define glPauseTransformFeedback_RealName GLEW_GET_FUN(__glewPauseTransformFeedback)
#define glPointParameterf_RealName GLEW_GET_FUN(__glewPointParameterf)
#define glPointParameterfv_RealName GLEW_GET_FUN(__glewPointParameterfv)
#define glPointParameteri_RealName GLEW_GET_FUN(__glewPointParameteri)
#define glPointParameteriv_RealName GLEW_GET_FUN(__glewPointParameteriv)
#define glPrimitiveRestartIndex_RealName GLEW_GET_FUN(__glewPrimitiveRestartIndex)
#define glProgramBinary_RealName GLEW_GET_FUN(__glewProgramBinary)
#define glProgramParameteri_RealName GLEW_GET_FUN(__glewProgramParameteri)
#define glProgramUniform1d_RealName GLEW_GET_FUN(__glewProgramUniform1d)
#define glProgramUniform1dv_RealName GLEW_GET_FUN(__glewProgramUniform1dv)
#define glProgramUniform1f_RealName GLEW_GET_FUN(__glewProgramUniform1f)
#define glProgramUniform1fv_RealName GLEW_GET_FUN(__glewProgramUniform1fv)
#define glProgramUniform1i_RealName GLEW_GET_FUN(__glewProgramUniform1i)
#define glProgramUniform1iv_RealName GLEW_GET_FUN(__glewProgramUniform1iv)
#define glProgramUniform1ui_RealName GLEW_GET_FUN(__glewProgramUniform1ui)
#define glProgramUniform1uiv_RealName GLEW_GET_FUN(__glewProgramUniform1uiv)
#define glProgramUniform2d_RealName GLEW_GET_FUN(__glewProgramUniform2d)
#define glProgramUniform2dv_RealName GLEW_GET_FUN(__glewProgramUniform2dv)
#define glProgramUniform2f_RealName GLEW_GET_FUN(__glewProgramUniform2f)
#define glProgramUniform2fv_RealName GLEW_GET_FUN(__glewProgramUniform2fv)
#define glProgramUniform2i_RealName GLEW_GET_FUN(__glewProgramUniform2i)
#define glProgramUniform2iv_RealName GLEW_GET_FUN(__glewProgramUniform2iv)
#define glProgramUniform2ui_RealName GLEW_GET_FUN(__glewProgramUniform2ui)
#define glProgramUniform2uiv_RealName GLEW_GET_FUN(__glewProgramUniform2uiv)
#define glProgramUniform3d_RealName GLEW_GET_FUN(__glewProgramUniform3d)
#define glProgramUniform3dv_RealName GLEW_GET_FUN(__glewProgramUniform3dv)
#define glProgramUniform3f_RealName GLEW_GET_FUN(__glewProgramUniform3f)
#define glProgramUniform3fv_RealName GLEW_GET_FUN(__glewProgramUniform3fv)
#define glProgramUniform3i_RealName GLEW_GET_FUN(__glewProgramUniform3i)
#define glProgramUniform3iv_RealName GLEW_GET_FUN(__glewProgramUniform3iv)
#define glProgramUniform3ui_RealName GLEW_GET_FUN(__glewProgramUniform3ui)
#define glProgramUniform3uiv_RealName GLEW_GET_FUN(__glewProgramUniform3uiv)
#define glProgramUniform4d_RealName GLEW_GET_FUN(__glewProgramUniform4d)
#define glProgramUniform4dv_RealName GLEW_GET_FUN(__glewProgramUniform4dv)
#define glProgramUniform4f_RealName GLEW_GET_FUN(__glewProgramUniform4f)
#define glProgramUniform4fv_RealName GLEW_GET_FUN(__glewProgramUniform4fv)
#define glProgramUniform4i_RealName GLEW_GET_FUN(__glewProgramUniform4i)
#define glProgramUniform4iv_RealName GLEW_GET_FUN(__glewProgramUniform4iv)
#define glProgramUniform4ui_RealName GLEW_GET_FUN(__glewProgramUniform4ui)
#define glProgramUniform4uiv_RealName GLEW_GET_FUN(__glewProgramUniform4uiv)
#define glProgramUniformMatrix2dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix2dv)
#define glProgramUniformMatrix2fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix2fv)
#define glProgramUniformMatrix2x3dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix2x3dv)
#define glProgramUniformMatrix2x3fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix2x3fv)
#define glProgramUniformMatrix2x4dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix2x4dv)
#define glProgramUniformMatrix2x4fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix2x4fv)
#define glProgramUniformMatrix3dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix3dv)
#define glProgramUniformMatrix3fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix3fv)
#define glProgramUniformMatrix3x2dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix3x2dv)
#define glProgramUniformMatrix3x2fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix3x2fv)
#define glProgramUniformMatrix3x4dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix3x4dv)
#define glProgramUniformMatrix3x4fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix3x4fv)
#define glProgramUniformMatrix4dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix4dv)
#define glProgramUniformMatrix4fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix4fv)
#define glProgramUniformMatrix4x2dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix4x2dv)
#define glProgramUniformMatrix4x2fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix4x2fv)
#define glProgramUniformMatrix4x3dv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix4x3dv)
#define glProgramUniformMatrix4x3fv_RealName GLEW_GET_FUN(__glewProgramUniformMatrix4x3fv)
#define glProvokingVertex_RealName GLEW_GET_FUN(__glewProvokingVertex)
#define glQueryCounter_RealName GLEW_GET_FUN(__glewQueryCounter)
#define glReleaseShaderCompiler_RealName GLEW_GET_FUN(__glewReleaseShaderCompiler)
#define glRenderbufferStorage_RealName GLEW_GET_FUN(__glewRenderbufferStorage)
#define glRenderbufferStorageMultisample_RealName GLEW_GET_FUN(__glewRenderbufferStorageMultisample)
#define glResumeTransformFeedback_RealName GLEW_GET_FUN(__glewResumeTransformFeedback)
#define glSampleCoverage_RealName GLEW_GET_FUN(__glewSampleCoverage)
#define glSampleMaski_RealName GLEW_GET_FUN(__glewSampleMaski)
#define glSamplerParameterIiv_RealName GLEW_GET_FUN(__glewSamplerParameterIiv)
#define glSamplerParameterIuiv_RealName GLEW_GET_FUN(__glewSamplerParameterIuiv)
#define glSamplerParameterf_RealName GLEW_GET_FUN(__glewSamplerParameterf)
#define glSamplerParameterfv_RealName GLEW_GET_FUN(__glewSamplerParameterfv)
#define glSamplerParameteri_RealName GLEW_GET_FUN(__glewSamplerParameteri)
#define glSamplerParameteriv_RealName GLEW_GET_FUN(__glewSamplerParameteriv)
#define glScissorArrayv_RealName GLEW_GET_FUN(__glewScissorArrayv)
#define glScissorIndexed_RealName GLEW_GET_FUN(__glewScissorIndexed)
#define glScissorIndexedv_RealName GLEW_GET_FUN(__glewScissorIndexedv)
#define glShaderBinary_RealName GLEW_GET_FUN(__glewShaderBinary)
#define glShaderSource_RealName GLEW_GET_FUN(__glewShaderSource)
#define glStencilFuncSeparate_RealName GLEW_GET_FUN(__glewStencilFuncSeparate)
#define glStencilMaskSeparate_RealName GLEW_GET_FUN(__glewStencilMaskSeparate)
#define glStencilOpSeparate_RealName GLEW_GET_FUN(__glewStencilOpSeparate)
#define glTexBuffer_RealName GLEW_GET_FUN(__glewTexBuffer)
#define glTexImage2DMultisample_RealName GLEW_GET_FUN(__glewTexImage2DMultisample)
#define glTexImage3D_RealName GLEW_GET_FUN(__glewTexImage3D)
#define glTexImage3DMultisample_RealName GLEW_GET_FUN(__glewTexImage3DMultisample)
#define glTexParameterIiv_RealName GLEW_GET_FUN(__glewTexParameterIiv)
#define glTexParameterIuiv_RealName GLEW_GET_FUN(__glewTexParameterIuiv)
#define glTexSubImage3D_RealName GLEW_GET_FUN(__glewTexSubImage3D)
#define glTransformFeedbackVaryings_RealName GLEW_GET_FUN(__glewTransformFeedbackVaryings)
#define glUniform1d_RealName GLEW_GET_FUN(__glewUniform1d)
#define glUniform1dv_RealName GLEW_GET_FUN(__glewUniform1dv)
#define glUniform1f_RealName GLEW_GET_FUN(__glewUniform1f)
#define glUniform1fv_RealName GLEW_GET_FUN(__glewUniform1fv)
#define glUniform1i_RealName GLEW_GET_FUN(__glewUniform1i)
#define glUniform1iv_RealName GLEW_GET_FUN(__glewUniform1iv)
#define glUniform1ui_RealName GLEW_GET_FUN(__glewUniform1ui)
#define glUniform1uiv_RealName GLEW_GET_FUN(__glewUniform1uiv)
#define glUniform2d_RealName GLEW_GET_FUN(__glewUniform2d)
#define glUniform2dv_RealName GLEW_GET_FUN(__glewUniform2dv)
#define glUniform2f_RealName GLEW_GET_FUN(__glewUniform2f)
#define glUniform2fv_RealName GLEW_GET_FUN(__glewUniform2fv)
#define glUniform2i_RealName GLEW_GET_FUN(__glewUniform2i)
#define glUniform2iv_RealName GLEW_GET_FUN(__glewUniform2iv)
#define glUniform2ui_RealName GLEW_GET_FUN(__glewUniform2ui)
#define glUniform2uiv_RealName GLEW_GET_FUN(__glewUniform2uiv)
#define glUniform3d_RealName GLEW_GET_FUN(__glewUniform3d)
#define glUniform3dv_RealName GLEW_GET_FUN(__glewUniform3dv)
#define glUniform3f_RealName GLEW_GET_FUN(__glewUniform3f)
#define glUniform3fv_RealName GLEW_GET_FUN(__glewUniform3fv)
#define glUniform3i_RealName GLEW_GET_FUN(__glewUniform3i)
#define glUniform3iv_RealName GLEW_GET_FUN(__glewUniform3iv)
#define glUniform3ui_RealName GLEW_GET_FUN(__glewUniform3ui)
#define glUniform3uiv_RealName GLEW_GET_FUN(__glewUniform3uiv)
#define glUniform4d_RealName GLEW_GET_FUN(__glewUniform4d)
#define glUniform4dv_RealName GLEW_GET_FUN(__glewUniform4dv)
#define glUniform4f_RealName GLEW_GET_FUN(__glewUniform4f)
#define glUniform4fv_RealName GLEW_GET_FUN(__glewUniform4fv)
#define glUniform4i_RealName GLEW_GET_FUN(__glewUniform4i)
#define glUniform4iv_RealName GLEW_GET_FUN(__glewUniform4iv)
#define glUniform4ui_RealName GLEW_GET_FUN(__glewUniform4ui)
#define glUniform4uiv_RealName GLEW_GET_FUN(__glewUniform4uiv)
#define glUniformBlockBinding_RealName GLEW_GET_FUN(__glewUniformBlockBinding)
#define glUniformMatrix2dv_RealName GLEW_GET_FUN(__glewUniformMatrix2dv)
#define glUniformMatrix2fv_RealName GLEW_GET_FUN(__glewUniformMatrix2fv)
#define glUniformMatrix2x3dv_RealName GLEW_GET_FUN(__glewUniformMatrix2x3dv)
#define glUniformMatrix2x3fv_RealName GLEW_GET_FUN(__glewUniformMatrix2x3fv)
#define glUniformMatrix2x4dv_RealName GLEW_GET_FUN(__glewUniformMatrix2x4dv)
#define glUniformMatrix2x4fv_RealName GLEW_GET_FUN(__glewUniformMatrix2x4fv)
#define glUniformMatrix3dv_RealName GLEW_GET_FUN(__glewUniformMatrix3dv)
#define glUniformMatrix3fv_RealName GLEW_GET_FUN(__glewUniformMatrix3fv)
#define glUniformMatrix3x2dv_RealName GLEW_GET_FUN(__glewUniformMatrix3x2dv)
#define glUniformMatrix3x2fv_RealName GLEW_GET_FUN(__glewUniformMatrix3x2fv)
#define glUniformMatrix3x4dv_RealName GLEW_GET_FUN(__glewUniformMatrix3x4dv)
#define glUniformMatrix3x4fv_RealName GLEW_GET_FUN(__glewUniformMatrix3x4fv)
#define glUniformMatrix4dv_RealName GLEW_GET_FUN(__glewUniformMatrix4dv)
#define glUniformMatrix4fv_RealName GLEW_GET_FUN(__glewUniformMatrix4fv)
#define glUniformMatrix4x2dv_RealName GLEW_GET_FUN(__glewUniformMatrix4x2dv)
#define glUniformMatrix4x2fv_RealName GLEW_GET_FUN(__glewUniformMatrix4x2fv)
#define glUniformMatrix4x3dv_RealName GLEW_GET_FUN(__glewUniformMatrix4x3dv)
#define glUniformMatrix4x3fv_RealName GLEW_GET_FUN(__glewUniformMatrix4x3fv)
#define glUniformSubroutinesuiv_RealName GLEW_GET_FUN(__glewUniformSubroutinesuiv)
#define glUnmapBuffer_RealName GLEW_GET_FUN(__glewUnmapBuffer)
#define glUseProgram_RealName GLEW_GET_FUN(__glewUseProgram)
#define glUseProgramStages_RealName GLEW_GET_FUN(__glewUseProgramStages)
#define glValidateProgram_RealName GLEW_GET_FUN(__glewValidateProgram)
#define glValidateProgramPipeline_RealName GLEW_GET_FUN(__glewValidateProgramPipeline)
#define glVertexAttrib1d_RealName GLEW_GET_FUN(__glewVertexAttrib1d)
#define glVertexAttrib1dv_RealName GLEW_GET_FUN(__glewVertexAttrib1dv)
#define glVertexAttrib1f_RealName GLEW_GET_FUN(__glewVertexAttrib1f)
#define glVertexAttrib1fv_RealName GLEW_GET_FUN(__glewVertexAttrib1fv)
#define glVertexAttrib1s_RealName GLEW_GET_FUN(__glewVertexAttrib1s)
#define glVertexAttrib1sv_RealName GLEW_GET_FUN(__glewVertexAttrib1sv)
#define glVertexAttrib2d_RealName GLEW_GET_FUN(__glewVertexAttrib2d)
#define glVertexAttrib2dv_RealName GLEW_GET_FUN(__glewVertexAttrib2dv)
#define glVertexAttrib2f_RealName GLEW_GET_FUN(__glewVertexAttrib2f)
#define glVertexAttrib2fv_RealName GLEW_GET_FUN(__glewVertexAttrib2fv)
#define glVertexAttrib2s_RealName GLEW_GET_FUN(__glewVertexAttrib2s)
#define glVertexAttrib2sv_RealName GLEW_GET_FUN(__glewVertexAttrib2sv)
#define glVertexAttrib3d_RealName GLEW_GET_FUN(__glewVertexAttrib3d)
#define glVertexAttrib3dv_RealName GLEW_GET_FUN(__glewVertexAttrib3dv)
#define glVertexAttrib3f_RealName GLEW_GET_FUN(__glewVertexAttrib3f)
#define glVertexAttrib3fv_RealName GLEW_GET_FUN(__glewVertexAttrib3fv)
#define glVertexAttrib3s_RealName GLEW_GET_FUN(__glewVertexAttrib3s)
#define glVertexAttrib3sv_RealName GLEW_GET_FUN(__glewVertexAttrib3sv)
#define glVertexAttrib4Nbv_RealName GLEW_GET_FUN(__glewVertexAttrib4Nbv)
#define glVertexAttrib4Niv_RealName GLEW_GET_FUN(__glewVertexAttrib4Niv)
#define glVertexAttrib4Nsv_RealName GLEW_GET_FUN(__glewVertexAttrib4Nsv)
#define glVertexAttrib4Nub_RealName GLEW_GET_FUN(__glewVertexAttrib4Nub)
#define glVertexAttrib4Nubv_RealName GLEW_GET_FUN(__glewVertexAttrib4Nubv)
#define glVertexAttrib4Nuiv_RealName GLEW_GET_FUN(__glewVertexAttrib4Nuiv)
#define glVertexAttrib4Nusv_RealName GLEW_GET_FUN(__glewVertexAttrib4Nusv)
#define glVertexAttrib4bv_RealName GLEW_GET_FUN(__glewVertexAttrib4bv)
#define glVertexAttrib4d_RealName GLEW_GET_FUN(__glewVertexAttrib4d)
#define glVertexAttrib4dv_RealName GLEW_GET_FUN(__glewVertexAttrib4dv)
#define glVertexAttrib4f_RealName GLEW_GET_FUN(__glewVertexAttrib4f)
#define glVertexAttrib4fv_RealName GLEW_GET_FUN(__glewVertexAttrib4fv)
#define glVertexAttrib4iv_RealName GLEW_GET_FUN(__glewVertexAttrib4iv)
#define glVertexAttrib4s_RealName GLEW_GET_FUN(__glewVertexAttrib4s)
#define glVertexAttrib4sv_RealName GLEW_GET_FUN(__glewVertexAttrib4sv)
#define glVertexAttrib4ubv_RealName GLEW_GET_FUN(__glewVertexAttrib4ubv)
#define glVertexAttrib4uiv_RealName GLEW_GET_FUN(__glewVertexAttrib4uiv)
#define glVertexAttrib4usv_RealName GLEW_GET_FUN(__glewVertexAttrib4usv)
#define glVertexAttribDivisor_RealName GLEW_GET_FUN(__glewVertexAttribDivisor)
#define glVertexAttribI1i_RealName GLEW_GET_FUN(__glewVertexAttribI1i)
#define glVertexAttribI1iv_RealName GLEW_GET_FUN(__glewVertexAttribI1iv)
#define glVertexAttribI1ui_RealName GLEW_GET_FUN(__glewVertexAttribI1ui)
#define glVertexAttribI1uiv_RealName GLEW_GET_FUN(__glewVertexAttribI1uiv)
#define glVertexAttribI2i_RealName GLEW_GET_FUN(__glewVertexAttribI2i)
#define glVertexAttribI2iv_RealName GLEW_GET_FUN(__glewVertexAttribI2iv)
#define glVertexAttribI2ui_RealName GLEW_GET_FUN(__glewVertexAttribI2ui)
#define glVertexAttribI2uiv_RealName GLEW_GET_FUN(__glewVertexAttribI2uiv)
#define glVertexAttribI3i_RealName GLEW_GET_FUN(__glewVertexAttribI3i)
#define glVertexAttribI3iv_RealName GLEW_GET_FUN(__glewVertexAttribI3iv)
#define glVertexAttribI3ui_RealName GLEW_GET_FUN(__glewVertexAttribI3ui)
#define glVertexAttribI3uiv_RealName GLEW_GET_FUN(__glewVertexAttribI3uiv)
#define glVertexAttribI4bv_RealName GLEW_GET_FUN(__glewVertexAttribI4bv)
#define glVertexAttribI4i_RealName GLEW_GET_FUN(__glewVertexAttribI4i)
#define glVertexAttribI4iv_RealName GLEW_GET_FUN(__glewVertexAttribI4iv)
#define glVertexAttribI4sv_RealName GLEW_GET_FUN(__glewVertexAttribI4sv)
#define glVertexAttribI4ubv_RealName GLEW_GET_FUN(__glewVertexAttribI4ubv)
#define glVertexAttribI4ui_RealName GLEW_GET_FUN(__glewVertexAttribI4ui)
#define glVertexAttribI4uiv_RealName GLEW_GET_FUN(__glewVertexAttribI4uiv)
#define glVertexAttribI4usv_RealName GLEW_GET_FUN(__glewVertexAttribI4usv)
#define glVertexAttribIPointer_RealName GLEW_GET_FUN(__glewVertexAttribIPointer)
#define glVertexAttribL1d_RealName GLEW_GET_FUN(__glewVertexAttribL1d)
#define glVertexAttribL1dv_RealName GLEW_GET_FUN(__glewVertexAttribL1dv)
#define glVertexAttribL2d_RealName GLEW_GET_FUN(__glewVertexAttribL2d)
#define glVertexAttribL2dv_RealName GLEW_GET_FUN(__glewVertexAttribL2dv)
#define glVertexAttribL3d_RealName GLEW_GET_FUN(__glewVertexAttribL3d)
#define glVertexAttribL3dv_RealName GLEW_GET_FUN(__glewVertexAttribL3dv)
#define glVertexAttribL4d_RealName GLEW_GET_FUN(__glewVertexAttribL4d)
#define glVertexAttribL4dv_RealName GLEW_GET_FUN(__glewVertexAttribL4dv)
#define glVertexAttribLPointer_RealName GLEW_GET_FUN(__glewVertexAttribLPointer)
#define glVertexAttribP1ui_RealName GLEW_GET_FUN(__glewVertexAttribP1ui)
#define glVertexAttribP1uiv_RealName GLEW_GET_FUN(__glewVertexAttribP1uiv)
#define glVertexAttribP2ui_RealName GLEW_GET_FUN(__glewVertexAttribP2ui)
#define glVertexAttribP2uiv_RealName GLEW_GET_FUN(__glewVertexAttribP2uiv)
#define glVertexAttribP3ui_RealName GLEW_GET_FUN(__glewVertexAttribP3ui)
#define glVertexAttribP3uiv_RealName GLEW_GET_FUN(__glewVertexAttribP3uiv)
#define glVertexAttribP4ui_RealName GLEW_GET_FUN(__glewVertexAttribP4ui)
#define glVertexAttribP4uiv_RealName GLEW_GET_FUN(__glewVertexAttribP4uiv)
#define glVertexAttribPointer_RealName GLEW_GET_FUN(__glewVertexAttribPointer)
#define glViewportArrayv_RealName GLEW_GET_FUN(__glewViewportArrayv)
#define glViewportIndexedf_RealName GLEW_GET_FUN(__glewViewportIndexedf)
#define glViewportIndexedfv_RealName GLEW_GET_FUN(__glewViewportIndexedfv)
#define glWaitSync_RealName GLEW_GET_FUN(__glewWaitSync)

#else // !defined(__GLEW_H__)

#define glActiveShaderProgram_RealName glActiveShaderProgram
#define glActiveTexture_RealName glActiveTexture
#define glAttachShader_RealName glAttachShader
#define glBeginConditionalRender_RealName glBeginConditionalRender
#define glBeginQuery_RealName glBeginQuery
#define glBeginQueryIndexed_RealName glBeginQueryIndexed
#define glBeginTransformFeedback_RealName glBeginTransformFeedback
#define glBindAttribLocation_RealName glBindAttribLocation
#define glBindBuffer_RealName glBindBuffer
#define glBindBufferBase_RealName glBindBufferBase
#define glBindBufferRange_RealName glBindBufferRange
#define glBindFragDataLocation_RealName glBindFragDataLocation
#define glBindFragDataLocationIndexed_RealName glBindFragDataLocationIndexed
#define glBindFramebuffer_RealName glBindFramebuffer
#define glBindProgramPipeline_RealName glBindProgramPipeline
#define glBindRenderbuffer_RealName glBindRenderbuffer
#define glBindSampler_RealName glBindSampler
#define glBindTransformFeedback_RealName glBindTransformFeedback
#define glBindVertexArray_RealName glBindVertexArray
#define glBlendColor_RealName glBlendColor
#define glBlendEquation_RealName glBlendEquation
#define glBlendEquationSeparate_RealName glBlendEquationSeparate
#define glBlendEquationSeparatei_RealName glBlendEquationSeparatei
#define glBlendEquationi_RealName glBlendEquationi
#define glBlendFuncSeparate_RealName glBlendFuncSeparate
#define glBlendFuncSeparatei_RealName glBlendFuncSeparatei
#define glBlendFunci_RealName glBlendFunci
#define glBlitFramebuffer_RealName glBlitFramebuffer
#define glBufferData_RealName glBufferData
#define glBufferSubData_RealName glBufferSubData
#define glCheckFramebufferStatus_RealName glCheckFramebufferStatus
#define glClampColor_RealName glClampColor
#define glClearBufferfi_RealName glClearBufferfi
#define glClearBufferfv_RealName glClearBufferfv
#define glClearBufferiv_RealName glClearBufferiv
#define glClearBufferuiv_RealName glClearBufferuiv
#define glClearDepthf_RealName glClearDepthf
#define glClientWaitSync_RealName glClientWaitSync
#define glColorMaski_RealName glColorMaski
#define glCompileShader_RealName glCompileShader
#define glCompressedTexImage1D_RealName glCompressedTexImage1D
#define glCompressedTexImage2D_RealName glCompressedTexImage2D
#define glCompressedTexImage3D_RealName glCompressedTexImage3D
#define glCompressedTexSubImage1D_RealName glCompressedTexSubImage1D
#define glCompressedTexSubImage2D_RealName glCompressedTexSubImage2D
#define glCompressedTexSubImage3D_RealName glCompressedTexSubImage3D
#define glCopyBufferSubData_RealName glCopyBufferSubData
#define glCopyTexSubImage3D_RealName glCopyTexSubImage3D
#define glCreateProgram_RealName glCreateProgram
#define glCreateShader_RealName glCreateShader
#define glCreateShaderProgramv_RealName glCreateShaderProgramv
#define glDeleteBuffers_RealName glDeleteBuffers
#define glDeleteFramebuffers_RealName glDeleteFramebuffers
#define glDeleteProgram_RealName glDeleteProgram
#define glDeleteProgramPipelines_RealName glDeleteProgramPipelines
#define glDeleteQueries_RealName glDeleteQueries
#define glDeleteRenderbuffers_RealName glDeleteRenderbuffers
#define glDeleteSamplers_RealName glDeleteSamplers
#define glDeleteShader_RealName glDeleteShader
#define glDeleteSync_RealName glDeleteSync
#define glDeleteTransformFeedbacks_RealName glDeleteTransformFeedbacks
#define glDeleteVertexArrays_RealName glDeleteVertexArrays
#define glDepthRangeArrayv_RealName glDepthRangeArrayv
#define glDepthRangeIndexed_RealName glDepthRangeIndexed
#define glDepthRangef_RealName glDepthRangef
#define glDetachShader_RealName glDetachShader
#define glDisableVertexAttribArray_RealName glDisableVertexAttribArray
#define glDisablei_RealName glDisablei
#define glDrawArraysIndirect_RealName glDrawArraysIndirect
#define glDrawArraysInstanced_RealName glDrawArraysInstanced
#define glDrawBuffers_RealName glDrawBuffers
#define glDrawElementsBaseVertex_RealName glDrawElementsBaseVertex
#define glDrawElementsIndirect_RealName glDrawElementsIndirect
#define glDrawElementsInstanced_RealName glDrawElementsInstanced
#define glDrawElementsInstancedBaseVertex_RealName glDrawElementsInstancedBaseVertex
#define glDrawRangeElements_RealName glDrawRangeElements
#define glDrawRangeElementsBaseVertex_RealName glDrawRangeElementsBaseVertex
#define glDrawTransformFeedback_RealName glDrawTransformFeedback
#define glDrawTransformFeedbackStream_RealName glDrawTransformFeedbackStream
#define glEnableVertexAttribArray_RealName glEnableVertexAttribArray
#define glEnablei_RealName glEnablei
#define glEndConditionalRender_RealName glEndConditionalRender
#define glEndQuery_RealName glEndQuery
#define glEndQueryIndexed_RealName glEndQueryIndexed
#define glEndTransformFeedback_RealName glEndTransformFeedback
#define glFenceSync_RealName glFenceSync
#define glFlushMappedBufferRange_RealName glFlushMappedBufferRange
#define glFramebufferRenderbuffer_RealName glFramebufferRenderbuffer
#define glFramebufferTexture_RealName glFramebufferTexture
#define glFramebufferTexture1D_RealName glFramebufferTexture1D
#define glFramebufferTexture2D_RealName glFramebufferTexture2D
#define glFramebufferTexture3D_RealName glFramebufferTexture3D
#define glFramebufferTextureLayer_RealName glFramebufferTextureLayer
#define glGenBuffers_RealName glGenBuffers
#define glGenFramebuffers_RealName glGenFramebuffers
#define glGenProgramPipelines_RealName glGenProgramPipelines
#define glGenQueries_RealName glGenQueries
#define glGenRenderbuffers_RealName glGenRenderbuffers
#define glGenSamplers_RealName glGenSamplers
#define glGenTransformFeedbacks_RealName glGenTransformFeedbacks
#define glGenVertexArrays_RealName glGenVertexArrays
#define glGenerateMipmap_RealName glGenerateMipmap
#define glGetActiveAttrib_RealName glGetActiveAttrib
#define glGetActiveSubroutineName_RealName glGetActiveSubroutineName
#define glGetActiveSubroutineUniformName_RealName glGetActiveSubroutineUniformName
#define glGetActiveSubroutineUniformiv_RealName glGetActiveSubroutineUniformiv
#define glGetActiveUniform_RealName glGetActiveUniform
#define glGetActiveUniformBlockName_RealName glGetActiveUniformBlockName
#define glGetActiveUniformBlockiv_RealName glGetActiveUniformBlockiv
#define glGetActiveUniformName_RealName glGetActiveUniformName
#define glGetActiveUniformsiv_RealName glGetActiveUniformsiv
#define glGetAttachedShaders_RealName glGetAttachedShaders
#define glGetAttribLocation_RealName glGetAttribLocation
#define glGetBooleani_v_RealName glGetBooleani_v
#define glGetBufferParameteri64v_RealName glGetBufferParameteri64v
#define glGetBufferParameteriv_RealName glGetBufferParameteriv
#define glGetBufferPointerv_RealName glGetBufferPointerv
#define glGetBufferSubData_RealName glGetBufferSubData
#define glGetCompressedTexImage_RealName glGetCompressedTexImage
#define glGetDoublei_v_RealName glGetDoublei_v
#define glGetFloati_v_RealName glGetFloati_v
#define glGetFragDataIndex_RealName glGetFragDataIndex
#define glGetFragDataLocation_RealName glGetFragDataLocation
#define glGetFramebufferAttachmentParameteriv_RealName glGetFramebufferAttachmentParameteriv
#define glGetInteger64i_v_RealName glGetInteger64i_v
#define glGetInteger64v_RealName glGetInteger64v
#define glGetIntegeri_v_RealName glGetIntegeri_v
#define glGetMultisamplefv_RealName glGetMultisamplefv
#define glGetProgramBinary_RealName glGetProgramBinary
#define glGetProgramInfoLog_RealName glGetProgramInfoLog
#define glGetProgramPipelineInfoLog_RealName glGetProgramPipelineInfoLog
#define glGetProgramPipelineiv_RealName glGetProgramPipelineiv
#define glGetProgramStageiv_RealName glGetProgramStageiv
#define glGetProgramiv_RealName glGetProgramiv
#define glGetQueryIndexediv_RealName glGetQueryIndexediv
#define glGetQueryObjecti64v_RealName glGetQueryObjecti64v
#define glGetQueryObjectiv_RealName glGetQueryObjectiv
#define glGetQueryObjectui64v_RealName glGetQueryObjectui64v
#define glGetQueryObjectuiv_RealName glGetQueryObjectuiv
#define glGetQueryiv_RealName glGetQueryiv
#define glGetRenderbufferParameteriv_RealName glGetRenderbufferParameteriv
#define glGetSamplerParameterIiv_RealName glGetSamplerParameterIiv
#define glGetSamplerParameterIuiv_RealName glGetSamplerParameterIuiv
#define glGetSamplerParameterfv_RealName glGetSamplerParameterfv
#define glGetSamplerParameteriv_RealName glGetSamplerParameteriv
#define glGetShaderInfoLog_RealName glGetShaderInfoLog
#define glGetShaderPrecisionFormat_RealName glGetShaderPrecisionFormat
#define glGetShaderSource_RealName glGetShaderSource
#define glGetShaderiv_RealName glGetShaderiv
#define glGetStringi_RealName glGetStringi
#define glGetSubroutineIndex_RealName glGetSubroutineIndex
#define glGetSubroutineUniformLocation_RealName glGetSubroutineUniformLocation
#define glGetSynciv_RealName glGetSynciv
#define glGetTexParameterIiv_RealName glGetTexParameterIiv
#define glGetTexParameterIuiv_RealName glGetTexParameterIuiv
#define glGetTransformFeedbackVarying_RealName glGetTransformFeedbackVarying
#define glGetUniformBlockIndex_RealName glGetUniformBlockIndex
#define glGetUniformIndices_RealName glGetUniformIndices
#define glGetUniformLocation_RealName glGetUniformLocation
#define glGetUniformSubroutineuiv_RealName glGetUniformSubroutineuiv
#define glGetUniformdv_RealName glGetUniformdv
#define glGetUniformfv_RealName glGetUniformfv
#define glGetUniformiv_RealName glGetUniformiv
#define glGetUniformuiv_RealName glGetUniformuiv
#define glGetVertexAttribIiv_RealName glGetVertexAttribIiv
#define glGetVertexAttribIuiv_RealName glGetVertexAttribIuiv
#define glGetVertexAttribLdv_RealName glGetVertexAttribLdv
#define glGetVertexAttribPointerv_RealName glGetVertexAttribPointerv
#define glGetVertexAttribdv_RealName glGetVertexAttribdv
#define glGetVertexAttribfv_RealName glGetVertexAttribfv
#define glGetVertexAttribiv_RealName glGetVertexAttribiv
#define glIsBuffer_RealName glIsBuffer
#define glIsEnabledi_RealName glIsEnabledi
#define glIsFramebuffer_RealName glIsFramebuffer
#define glIsProgram_RealName glIsProgram
#define glIsProgramPipeline_RealName glIsProgramPipeline
#define glIsQuery_RealName glIsQuery
#define glIsRenderbuffer_RealName glIsRenderbuffer
#define glIsSampler_RealName glIsSampler
#define glIsShader_RealName glIsShader
#define glIsSync_RealName glIsSync
#define glIsTransformFeedback_RealName glIsTransformFeedback
#define glIsVertexArray_RealName glIsVertexArray
#define glLinkProgram_RealName glLinkProgram
#define glMapBuffer_RealName glMapBuffer
#define glMapBufferRange_RealName glMapBufferRange
#define glMinSampleShading_RealName glMinSampleShading
#define glMultiDrawArrays_RealName glMultiDrawArrays
#define glMultiDrawElements_RealName glMultiDrawElements
#define glMultiDrawElementsBaseVertex_RealName glMultiDrawElementsBaseVertex
#define glPatchParameterfv_RealName glPatchParameterfv
#define glPatchParameteri_RealName glPatchParameteri
#define glPauseTransformFeedback_RealName glPauseTransformFeedback
#define glPointParameterf_RealName glPointParameterf
#define glPointParameterfv_RealName glPointParameterfv
#define glPointParameteri_RealName glPointParameteri
#define glPointParameteriv_RealName glPointParameteriv
#define glPrimitiveRestartIndex_RealName glPrimitiveRestartIndex
#define glProgramBinary_RealName glProgramBinary
#define glProgramParameteri_RealName glProgramParameteri
#define glProgramUniform1d_RealName glProgramUniform1d
#define glProgramUniform1dv_RealName glProgramUniform1dv
#define glProgramUniform1f_RealName glProgramUniform1f
#define glProgramUniform1fv_RealName glProgramUniform1fv
#define glProgramUniform1i_RealName glProgramUniform1i
#define glProgramUniform1iv_RealName glProgramUniform1iv
#define glProgramUniform1ui_RealName glProgramUniform1ui
#define glProgramUniform1uiv_RealName glProgramUniform1uiv
#define glProgramUniform2d_RealName glProgramUniform2d
#define glProgramUniform2dv_RealName glProgramUniform2dv
#define glProgramUniform2f_RealName glProgramUniform2f
#define glProgramUniform2fv_RealName glProgramUniform2fv
#define glProgramUniform2i_RealName glProgramUniform2i
#define glProgramUniform2iv_RealName glProgramUniform2iv
#define glProgramUniform2ui_RealName glProgramUniform2ui
#define glProgramUniform2uiv_RealName glProgramUniform2uiv
#define glProgramUniform3d_RealName glProgramUniform3d
#define glProgramUniform3dv_RealName glProgramUniform3dv
#define glProgramUniform3f_RealName glProgramUniform3f
#define glProgramUniform3fv_RealName glProgramUniform3fv
#define glProgramUniform3i_RealName glProgramUniform3i
#define glProgramUniform3iv_RealName glProgramUniform3iv
#define glProgramUniform3ui_RealName glProgramUniform3ui
#define glProgramUniform3uiv_RealName glProgramUniform3uiv
#define glProgramUniform4d_RealName glProgramUniform4d
#define glProgramUniform4dv_RealName glProgramUniform4dv
#define glProgramUniform4f_RealName glProgramUniform4f
#define glProgramUniform4fv_RealName glProgramUniform4fv
#define glProgramUniform4i_RealName glProgramUniform4i
#define glProgramUniform4iv_RealName glProgramUniform4iv
#define glProgramUniform4ui_RealName glProgramUniform4ui
#define glProgramUniform4uiv_RealName glProgramUniform4uiv
#define glProgramUniformMatrix2dv_RealName glProgramUniformMatrix2dv
#define glProgramUniformMatrix2fv_RealName glProgramUniformMatrix2fv
#define glProgramUniformMatrix2x3dv_RealName glProgramUniformMatrix2x3dv
#define glProgramUniformMatrix2x3fv_RealName glProgramUniformMatrix2x3fv
#define glProgramUniformMatrix2x4dv_RealName glProgramUniformMatrix2x4dv
#define glProgramUniformMatrix2x4fv_RealName glProgramUniformMatrix2x4fv
#define glProgramUniformMatrix3dv_RealName glProgramUniformMatrix3dv
#define glProgramUniformMatrix3fv_RealName glProgramUniformMatrix3fv
#define glProgramUniformMatrix3x2dv_RealName glProgramUniformMatrix3x2dv
#define glProgramUniformMatrix3x2fv_RealName glProgramUniformMatrix3x2fv
#define glProgramUniformMatrix3x4dv_RealName glProgramUniformMatrix3x4dv
#define glProgramUniformMatrix3x4fv_RealName glProgramUniformMatrix3x4fv
#define glProgramUniformMatrix4dv_RealName glProgramUniformMatrix4dv
#define glProgramUniformMatrix4fv_RealName glProgramUniformMatrix4fv
#define glProgramUniformMatrix4x2dv_RealName glProgramUniformMatrix4x2dv
#define glProgramUniformMatrix4x2fv_RealName glProgramUniformMatrix4x2fv
#define glProgramUniformMatrix4x3dv_RealName glProgramUniformMatrix4x3dv
#define glProgramUniformMatrix4x3fv_RealName glProgramUniformMatrix4x3fv
#define glProvokingVertex_RealName glProvokingVertex
#define glQueryCounter_RealName glQueryCounter
#define glReleaseShaderCompiler_RealName glReleaseShaderCompiler
#define glRenderbufferStorage_RealName glRenderbufferStorage
#define glRenderbufferStorageMultisample_RealName glRenderbufferStorageMultisample
#define glResumeTransformFeedback_RealName glResumeTransformFeedback
#define glSampleCoverage_RealName glSampleCoverage
#define glSampleMaski_RealName glSampleMaski
#define glSamplerParameterIiv_RealName glSamplerParameterIiv
#define glSamplerParameterIuiv_RealName glSamplerParameterIuiv
#define glSamplerParameterf_RealName glSamplerParameterf
#define glSamplerParameterfv_RealName glSamplerParameterfv
#define glSamplerParameteri_RealName glSamplerParameteri
#define glSamplerParameteriv_RealName glSamplerParameteriv
#define glScissorArrayv_RealName glScissorArrayv
#define glScissorIndexed_RealName glScissorIndexed
#define glScissorIndexedv_RealName glScissorIndexedv
#define glShaderBinary_RealName glShaderBinary
#define glShaderSource_RealName glShaderSource
#define glStencilFuncSeparate_RealName glStencilFuncSeparate
#define glStencilMaskSeparate_RealName glStencilMaskSeparate
#define glStencilOpSeparate_RealName glStencilOpSeparate
#define glTexBuffer_RealName glTexBuffer
#define glTexImage2DMultisample_RealName glTexImage2DMultisample
#define glTexImage3D_RealName glTexImage3D
#define glTexImage3DMultisample_RealName glTexImage3DMultisample
#define glTexParameterIiv_RealName glTexParameterIiv
#define glTexParameterIuiv_RealName glTexParameterIuiv
#define glTexSubImage3D_RealName glTexSubImage3D
#define glTransformFeedbackVaryings_RealName glTransformFeedbackVaryings
#define glUniform1d_RealName glUniform1d
#define glUniform1dv_RealName glUniform1dv
#define glUniform1f_RealName glUniform1f
#define glUniform1fv_RealName glUniform1fv
#define glUniform1i_RealName glUniform1i
#define glUniform1iv_RealName glUniform1iv
#define glUniform1ui_RealName glUniform1ui
#define glUniform1uiv_RealName glUniform1uiv
#define glUniform2d_RealName glUniform2d
#define glUniform2dv_RealName glUniform2dv
#define glUniform2f_RealName glUniform2f
#define glUniform2fv_RealName glUniform2fv
#define glUniform2i_RealName glUniform2i
#define glUniform2iv_RealName glUniform2iv
#define glUniform2ui_RealName glUniform2ui
#define glUniform2uiv_RealName glUniform2uiv
#define glUniform3d_RealName glUniform3d
#define glUniform3dv_RealName glUniform3dv
#define glUniform3f_RealName glUniform3f
#define glUniform3fv_RealName glUniform3fv
#define glUniform3i_RealName glUniform3i
#define glUniform3iv_RealName glUniform3iv
#define glUniform3ui_RealName glUniform3ui
#define glUniform3uiv_RealName glUniform3uiv
#define glUniform4d_RealName glUniform4d
#define glUniform4dv_RealName glUniform4dv
#define glUniform4f_RealName glUniform4f
#define glUniform4fv_RealName glUniform4fv
#define glUniform4i_RealName glUniform4i
#define glUniform4iv_RealName glUniform4iv
#define glUniform4ui_RealName glUniform4ui
#define glUniform4uiv_RealName glUniform4uiv
#define glUniformBlockBinding_RealName glUniformBlockBinding
#define glUniformMatrix2dv_RealName glUniformMatrix2dv
#define glUniformMatrix2fv_RealName glUniformMatrix2fv
#define glUniformMatrix2x3dv_RealName glUniformMatrix2x3dv
#define glUniformMatrix2x3fv_RealName glUniformMatrix2x3fv
#define glUniformMatrix2x4dv_RealName glUniformMatrix2x4dv
#define glUniformMatrix2x4fv_RealName glUniformMatrix2x4fv
#define glUniformMatrix3dv_RealName glUniformMatrix3dv
#define glUniformMatrix3fv_RealName glUniformMatrix3fv
#define glUniformMatrix3x2dv_RealName glUniformMatrix3x2dv
#define glUniformMatrix3x2fv_RealName glUniformMatrix3x2fv
#define glUniformMatrix3x4dv_RealName glUniformMatrix3x4dv
#define glUniformMatrix3x4fv_RealName glUniformMatrix3x4fv
#define glUniformMatrix4dv_RealName glUniformMatrix4dv
#define glUniformMatrix4fv_RealName glUniformMatrix4fv
#define glUniformMatrix4x2dv_RealName glUniformMatrix4x2dv
#define glUniformMatrix4x2fv_RealName glUniformMatrix4x2fv
#define glUniformMatrix4x3dv_RealName glUniformMatrix4x3dv
#define glUniformMatrix4x3fv_RealName glUniformMatrix4x3fv
#define glUniformSubroutinesuiv_RealName glUniformSubroutinesuiv
#define glUnmapBuffer_RealName glUnmapBuffer
#define glUseProgram_RealName glUseProgram
#define glUseProgramStages_RealName glUseProgramStages
#define glValidateProgram_RealName glValidateProgram
#define glValidateProgramPipeline_RealName glValidateProgramPipeline
#define glVertexAttrib1d_RealName glVertexAttrib1d
#define glVertexAttrib1dv_RealName glVertexAttrib1dv
#define glVertexAttrib1f_RealName glVertexAttrib1f
#define glVertexAttrib1fv_RealName glVertexAttrib1fv
#define glVertexAttrib1s_RealName glVertexAttrib1s
#define glVertexAttrib1sv_RealName glVertexAttrib1sv
#define glVertexAttrib2d_RealName glVertexAttrib2d
#define glVertexAttrib2dv_RealName glVertexAttrib2dv
#define glVertexAttrib2f_RealName glVertexAttrib2f
#define glVertexAttrib2fv_RealName glVertexAttrib2fv
#define glVertexAttrib2s_RealName glVertexAttrib2s
#define glVertexAttrib2sv_RealName glVertexAttrib2sv
#define glVertexAttrib3d_RealName glVertexAttrib3d
#define glVertexAttrib3dv_RealName glVertexAttrib3dv
#define glVertexAttrib3f_RealName glVertexAttrib3f
#define glVertexAttrib3fv_RealName glVertexAttrib3fv
#define glVertexAttrib3s_RealName glVertexAttrib3s
#define glVertexAttrib3sv_RealName glVertexAttrib3sv
#define glVertexAttrib4Nbv_RealName glVertexAttrib4Nbv
#define glVertexAttrib4Niv_RealName glVertexAttrib4Niv
#define glVertexAttrib4Nsv_RealName glVertexAttrib4Nsv
#define glVertexAttrib4Nub_RealName glVertexAttrib4Nub
#define glVertexAttrib4Nubv_RealName glVertexAttrib4Nubv
#define glVertexAttrib4Nuiv_RealName glVertexAttrib4Nuiv
#define glVertexAttrib4Nusv_RealName glVertexAttrib4Nusv
#define glVertexAttrib4bv_RealName glVertexAttrib4bv
#define glVertexAttrib4d_RealName glVertexAttrib4d
#define glVertexAttrib4dv_RealName glVertexAttrib4dv
#define glVertexAttrib4f_RealName glVertexAttrib4f
#define glVertexAttrib4fv_RealName glVertexAttrib4fv
#define glVertexAttrib4iv_RealName glVertexAttrib4iv
#define glVertexAttrib4s_RealName glVertexAttrib4s
#define glVertexAttrib4sv_RealName glVertexAttrib4sv
#define glVertexAttrib4ubv_RealName glVertexAttrib4ubv
#define glVertexAttrib4uiv_RealName glVertexAttrib4uiv
#define glVertexAttrib4usv_RealName glVertexAttrib4usv
#define glVertexAttribDivisor_RealName glVertexAttribDivisor
#define glVertexAttribI1i_RealName glVertexAttribI1i
#define glVertexAttribI1iv_RealName glVertexAttribI1iv
#define glVertexAttribI1ui_RealName glVertexAttribI1ui
#define glVertexAttribI1uiv_RealName glVertexAttribI1uiv
#define glVertexAttribI2i_RealName glVertexAttribI2i
#define glVertexAttribI2iv_RealName glVertexAttribI2iv
#define glVertexAttribI2ui_RealName glVertexAttribI2ui
#define glVertexAttribI2uiv_RealName glVertexAttribI2uiv
#define glVertexAttribI3i_RealName glVertexAttribI3i
#define glVertexAttribI3iv_RealName glVertexAttribI3iv
#define glVertexAttribI3ui_RealName glVertexAttribI3ui
#define glVertexAttribI3uiv_RealName glVertexAttribI3uiv
#define glVertexAttribI4bv_RealName glVertexAttribI4bv
#define glVertexAttribI4i_RealName glVertexAttribI4i
#define glVertexAttribI4iv_RealName glVertexAttribI4iv
#define glVertexAttribI4sv_RealName glVertexAttribI4sv
#define glVertexAttribI4ubv_RealName glVertexAttribI4ubv
#define glVertexAttribI4ui_RealName glVertexAttribI4ui
#define glVertexAttribI4uiv_RealName glVertexAttribI4uiv
#define glVertexAttribI4usv_RealName glVertexAttribI4usv
#define glVertexAttribIPointer_RealName glVertexAttribIPointer
#define glVertexAttribL1d_RealName glVertexAttribL1d
#define glVertexAttribL1dv_RealName glVertexAttribL1dv
#define glVertexAttribL2d_RealName glVertexAttribL2d
#define glVertexAttribL2dv_RealName glVertexAttribL2dv
#define glVertexAttribL3d_RealName glVertexAttribL3d
#define glVertexAttribL3dv_RealName glVertexAttribL3dv
#define glVertexAttribL4d_RealName glVertexAttribL4d
#define glVertexAttribL4dv_RealName glVertexAttribL4dv
#define glVertexAttribLPointer_RealName glVertexAttribLPointer
#define glVertexAttribP1ui_RealName glVertexAttribP1ui
#define glVertexAttribP1uiv_RealName glVertexAttribP1uiv
#define glVertexAttribP2ui_RealName glVertexAttribP2ui
#define glVertexAttribP2uiv_RealName glVertexAttribP2uiv
#define glVertexAttribP3ui_RealName glVertexAttribP3ui
#define glVertexAttribP3uiv_RealName glVertexAttribP3uiv
#define glVertexAttribP4ui_RealName glVertexAttribP4ui
#define glVertexAttribP4uiv_RealName glVertexAttribP4uiv
#define glVertexAttribPointer_RealName glVertexAttribPointer
#define glViewportArrayv_RealName glViewportArrayv
#define glViewportIndexedf_RealName glViewportIndexedf
#define glViewportIndexedfv_RealName glViewportIndexedfv
#define glWaitSync_RealName glWaitSync

#endif // defined(__GLEW_H_)


#endif // defined(GLEE_OVERWRITE_GL_FUNCTIONS)

////////////////////////////////////////////////////////////////////////////////
///  (Automatically generated by gen_glee.py)


#if defined(GLEE_OVERWRITE_GL_FUNCTIONS)

///  Calls `oglfun` with the arguments `...` in between two
/// `AssertNoOpenGLErrors` calls that are intended catch errors that occur at
/// or near the call of `oglfun`. This version of the macro is used for OpenGL
/// functions with no return value.
#define GLEE_GuardedGLCall(ogl_func, Type, real_func, check_arg, ...) ({\
    glee_api::AssertNoOpenGLErrors("some function before calling " #ogl_func " threw an error", __FUNCTION__, __FILE__, __LINE__, #ogl_func); \
    real_func ( __VA_ARGS__ ); \
    glee_api::AssertNoOpenGLErrors( #ogl_func " threw an error", __FUNCTION__, __FILE__, __LINE__, #ogl_func); \
    if (GLEE_DO_CHECK_UNIFORMS && check_arg <= -1) {\
        std::cerr << "{" << __FILE__ << ":" << __LINE__\
         << " (" << __FUNCTION__\
         << ")} error: [GLEE_CHECK_UNIFORM_LOCATIONS] " #ogl_func " received a negative value for its 'location' parameter (" << check_arg << ")" << std::endl;\
        assert(false);\
    }\
})

///  Calls `oglfun` with the arguments `...` in between two
/// `AssertNoOpenGLErrors` calls that are intended catch errors that occur at
/// or near the call of `oglfun`. This version of the macro is used for OpenGL
/// functions with a return value.
#define GLEE_GuardedGLCallWithReturn(ogl_func, Type, real_func, check_arg, ...) ({\
    glee_api::AssertNoOpenGLErrors("some function before calling " #ogl_func " threw an error", __FUNCTION__, __FILE__, __LINE__, #ogl_func); \
    Type __result = real_func ( __VA_ARGS__ ); \
    glee_api::AssertNoOpenGLErrors( #ogl_func " threw an error", __FUNCTION__, __FILE__, __LINE__, #ogl_func); \
    if (GLEE_DO_CHECK_UNIFORMS && check_arg <= -1) {\
        std::cerr << "{" << __FILE__ << ":" << __LINE__\
         << " (" << __FUNCTION__\
         << ")} error: [GLEE_CHECK_UNIFORM_LOCATIONS] " #ogl_func " received a negative value for its 'location' parameter (" << check_arg << ")" << std::endl;\
        assert(false);\
    }\
    __result;\
})

#define glAccum(op, value) GLEE_GuardedGLCall(glAccum, void, glAccum_RealName, 0, op, value)
#define glActiveShaderProgram(pipeline, program) GLEE_GuardedGLCall(glActiveShaderProgram, void, glActiveShaderProgram_RealName, 0, pipeline, program)
#define glActiveTexture(texture) GLEE_GuardedGLCall(glActiveTexture, void, glActiveTexture_RealName, 0, texture)
#define glAlphaFunc(func, ref) GLEE_GuardedGLCall(glAlphaFunc, void, glAlphaFunc_RealName, 0, func, ref)
#define glAreTexturesResident(n, textures, residences) GLEE_GuardedGLCallWithReturn(glAreTexturesResident, GLboolean, glAreTexturesResident_RealName, 0, n, textures, residences)
#define glArrayElement(i) GLEE_GuardedGLCall(glArrayElement, void, glArrayElement_RealName, 0, i)
#define glAttachShader(program, shader) GLEE_GuardedGLCall(glAttachShader, void, glAttachShader_RealName, 0, program, shader)
#define glBegin(mode) GLEE_GuardedGLCall(glBegin, void, glBegin_RealName, 0, mode)
#define glBeginConditionalRender(id, mode) GLEE_GuardedGLCall(glBeginConditionalRender, void, glBeginConditionalRender_RealName, 0, id, mode)
#define glBeginQuery(target, id) GLEE_GuardedGLCall(glBeginQuery, void, glBeginQuery_RealName, 0, target, id)
#define glBeginQueryIndexed(target, index, id) GLEE_GuardedGLCall(glBeginQueryIndexed, void, glBeginQueryIndexed_RealName, 0, target, index, id)
#define glBeginTransformFeedback(primitiveMode) GLEE_GuardedGLCall(glBeginTransformFeedback, void, glBeginTransformFeedback_RealName, 0, primitiveMode)
#define glBindAttribLocation(program, index, name) GLEE_GuardedGLCall(glBindAttribLocation, void, glBindAttribLocation_RealName, 0, program, index, name)
#define glBindBuffer(target, buffer) GLEE_GuardedGLCall(glBindBuffer, void, glBindBuffer_RealName, 0, target, buffer)
#define glBindBufferBase(target, index, buffer) GLEE_GuardedGLCall(glBindBufferBase, void, glBindBufferBase_RealName, 0, target, index, buffer)
#define glBindBufferRange(target, index, buffer, offset, size) GLEE_GuardedGLCall(glBindBufferRange, void, glBindBufferRange_RealName, 0, target, index, buffer, offset, size)
#define glBindFragDataLocation(program, color, name) GLEE_GuardedGLCall(glBindFragDataLocation, void, glBindFragDataLocation_RealName, 0, program, color, name)
#define glBindFragDataLocationIndexed(program, colorNumber, index, name) GLEE_GuardedGLCall(glBindFragDataLocationIndexed, void, glBindFragDataLocationIndexed_RealName, 0, program, colorNumber, index, name)
#define glBindFramebuffer(target, framebuffer) GLEE_GuardedGLCall(glBindFramebuffer, void, glBindFramebuffer_RealName, 0, target, framebuffer)
#define glBindProgramPipeline(pipeline) GLEE_GuardedGLCall(glBindProgramPipeline, void, glBindProgramPipeline_RealName, 0, pipeline)
#define glBindRenderbuffer(target, renderbuffer) GLEE_GuardedGLCall(glBindRenderbuffer, void, glBindRenderbuffer_RealName, 0, target, renderbuffer)
#define glBindSampler(unit, sampler) GLEE_GuardedGLCall(glBindSampler, void, glBindSampler_RealName, 0, unit, sampler)
#define glBindTexture(target, texture) GLEE_GuardedGLCall(glBindTexture, void, glBindTexture_RealName, 0, target, texture)
#define glBindTransformFeedback(target, id) GLEE_GuardedGLCall(glBindTransformFeedback, void, glBindTransformFeedback_RealName, 0, target, id)
#define glBindVertexArray(array) GLEE_GuardedGLCall(glBindVertexArray, void, glBindVertexArray_RealName, 0, array)
#define glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap) GLEE_GuardedGLCall(glBitmap, void, glBitmap_RealName, 0, width, height, xorig, yorig, xmove, ymove, bitmap)
#define glBlendColor(red, green, blue, alpha) GLEE_GuardedGLCall(glBlendColor, void, glBlendColor_RealName, 0, red, green, blue, alpha)
#define glBlendEquation(mode) GLEE_GuardedGLCall(glBlendEquation, void, glBlendEquation_RealName, 0, mode)
#define glBlendEquationSeparate(modeRGB, modeAlpha) GLEE_GuardedGLCall(glBlendEquationSeparate, void, glBlendEquationSeparate_RealName, 0, modeRGB, modeAlpha)
#define glBlendEquationSeparatei(buf, modeRGB, modeAlpha) GLEE_GuardedGLCall(glBlendEquationSeparatei, void, glBlendEquationSeparatei_RealName, 0, buf, modeRGB, modeAlpha)
#define glBlendEquationi(buf, mode) GLEE_GuardedGLCall(glBlendEquationi, void, glBlendEquationi_RealName, 0, buf, mode)
#define glBlendFunc(sfactor, dfactor) GLEE_GuardedGLCall(glBlendFunc, void, glBlendFunc_RealName, 0, sfactor, dfactor)
#define glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha) GLEE_GuardedGLCall(glBlendFuncSeparate, void, glBlendFuncSeparate_RealName, 0, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha)
#define glBlendFuncSeparatei(buf, srcRGB, dstRGB, srcAlpha, dstAlpha) GLEE_GuardedGLCall(glBlendFuncSeparatei, void, glBlendFuncSeparatei_RealName, 0, buf, srcRGB, dstRGB, srcAlpha, dstAlpha)
#define glBlendFunci(buf, src, dst) GLEE_GuardedGLCall(glBlendFunci, void, glBlendFunci_RealName, 0, buf, src, dst)
#define glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter) GLEE_GuardedGLCall(glBlitFramebuffer, void, glBlitFramebuffer_RealName, 0, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter)
#define glBufferData(target, size, data, usage) GLEE_GuardedGLCall(glBufferData, void, glBufferData_RealName, 0, target, size, data, usage)
#define glBufferSubData(target, offset, size, data) GLEE_GuardedGLCall(glBufferSubData, void, glBufferSubData_RealName, 0, target, offset, size, data)
#define glCallList(list) GLEE_GuardedGLCall(glCallList, void, glCallList_RealName, 0, list)
#define glCallLists(n, type, lists) GLEE_GuardedGLCall(glCallLists, void, glCallLists_RealName, 0, n, type, lists)
#define glCheckFramebufferStatus(target) GLEE_GuardedGLCallWithReturn(glCheckFramebufferStatus, GLenum, glCheckFramebufferStatus_RealName, 0, target)
#define glClampColor(target, clamp) GLEE_GuardedGLCall(glClampColor, void, glClampColor_RealName, 0, target, clamp)
#define glClear(mask) GLEE_GuardedGLCall(glClear, void, glClear_RealName, 0, mask)
#define glClearAccum(red, green, blue, alpha) GLEE_GuardedGLCall(glClearAccum, void, glClearAccum_RealName, 0, red, green, blue, alpha)
#define glClearBufferfi(buffer, drawbuffer, depth, stencil) GLEE_GuardedGLCall(glClearBufferfi, void, glClearBufferfi_RealName, 0, buffer, drawbuffer, depth, stencil)
#define glClearBufferfv(buffer, drawbuffer, value) GLEE_GuardedGLCall(glClearBufferfv, void, glClearBufferfv_RealName, 0, buffer, drawbuffer, value)
#define glClearBufferiv(buffer, drawbuffer, value) GLEE_GuardedGLCall(glClearBufferiv, void, glClearBufferiv_RealName, 0, buffer, drawbuffer, value)
#define glClearBufferuiv(buffer, drawbuffer, value) GLEE_GuardedGLCall(glClearBufferuiv, void, glClearBufferuiv_RealName, 0, buffer, drawbuffer, value)
#define glClearColor(red, green, blue, alpha) GLEE_GuardedGLCall(glClearColor, void, glClearColor_RealName, 0, red, green, blue, alpha)
#define glClearDepth(depth) GLEE_GuardedGLCall(glClearDepth, void, glClearDepth_RealName, 0, depth)
#define glClearDepthf(d) GLEE_GuardedGLCall(glClearDepthf, void, glClearDepthf_RealName, 0, d)
#define glClearIndex(c) GLEE_GuardedGLCall(glClearIndex, void, glClearIndex_RealName, 0, c)
#define glClearStencil(s) GLEE_GuardedGLCall(glClearStencil, void, glClearStencil_RealName, 0, s)
#define glClientWaitSync(sync, flags, timeout) GLEE_GuardedGLCallWithReturn(glClientWaitSync, GLenum, glClientWaitSync_RealName, 0, sync, flags, timeout)
#define glClipPlane(plane, equation) GLEE_GuardedGLCall(glClipPlane, void, glClipPlane_RealName, 0, plane, equation)
#define glColor3b(red, green, blue) GLEE_GuardedGLCall(glColor3b, void, glColor3b_RealName, 0, red, green, blue)
#define glColor3bv(v) GLEE_GuardedGLCall(glColor3bv, void, glColor3bv_RealName, 0, v)
#define glColor3d(red, green, blue) GLEE_GuardedGLCall(glColor3d, void, glColor3d_RealName, 0, red, green, blue)
#define glColor3dv(v) GLEE_GuardedGLCall(glColor3dv, void, glColor3dv_RealName, 0, v)
#define glColor3f(red, green, blue) GLEE_GuardedGLCall(glColor3f, void, glColor3f_RealName, 0, red, green, blue)
#define glColor3fv(v) GLEE_GuardedGLCall(glColor3fv, void, glColor3fv_RealName, 0, v)
#define glColor3i(red, green, blue) GLEE_GuardedGLCall(glColor3i, void, glColor3i_RealName, 0, red, green, blue)
#define glColor3iv(v) GLEE_GuardedGLCall(glColor3iv, void, glColor3iv_RealName, 0, v)
#define glColor3s(red, green, blue) GLEE_GuardedGLCall(glColor3s, void, glColor3s_RealName, 0, red, green, blue)
#define glColor3sv(v) GLEE_GuardedGLCall(glColor3sv, void, glColor3sv_RealName, 0, v)
#define glColor3ub(red, green, blue) GLEE_GuardedGLCall(glColor3ub, void, glColor3ub_RealName, 0, red, green, blue)
#define glColor3ubv(v) GLEE_GuardedGLCall(glColor3ubv, void, glColor3ubv_RealName, 0, v)
#define glColor3ui(red, green, blue) GLEE_GuardedGLCall(glColor3ui, void, glColor3ui_RealName, 0, red, green, blue)
#define glColor3uiv(v) GLEE_GuardedGLCall(glColor3uiv, void, glColor3uiv_RealName, 0, v)
#define glColor3us(red, green, blue) GLEE_GuardedGLCall(glColor3us, void, glColor3us_RealName, 0, red, green, blue)
#define glColor3usv(v) GLEE_GuardedGLCall(glColor3usv, void, glColor3usv_RealName, 0, v)
#define glColor4b(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4b, void, glColor4b_RealName, 0, red, green, blue, alpha)
#define glColor4bv(v) GLEE_GuardedGLCall(glColor4bv, void, glColor4bv_RealName, 0, v)
#define glColor4d(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4d, void, glColor4d_RealName, 0, red, green, blue, alpha)
#define glColor4dv(v) GLEE_GuardedGLCall(glColor4dv, void, glColor4dv_RealName, 0, v)
#define glColor4f(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4f, void, glColor4f_RealName, 0, red, green, blue, alpha)
#define glColor4fv(v) GLEE_GuardedGLCall(glColor4fv, void, glColor4fv_RealName, 0, v)
#define glColor4i(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4i, void, glColor4i_RealName, 0, red, green, blue, alpha)
#define glColor4iv(v) GLEE_GuardedGLCall(glColor4iv, void, glColor4iv_RealName, 0, v)
#define glColor4s(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4s, void, glColor4s_RealName, 0, red, green, blue, alpha)
#define glColor4sv(v) GLEE_GuardedGLCall(glColor4sv, void, glColor4sv_RealName, 0, v)
#define glColor4ub(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4ub, void, glColor4ub_RealName, 0, red, green, blue, alpha)
#define glColor4ubv(v) GLEE_GuardedGLCall(glColor4ubv, void, glColor4ubv_RealName, 0, v)
#define glColor4ui(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4ui, void, glColor4ui_RealName, 0, red, green, blue, alpha)
#define glColor4uiv(v) GLEE_GuardedGLCall(glColor4uiv, void, glColor4uiv_RealName, 0, v)
#define glColor4us(red, green, blue, alpha) GLEE_GuardedGLCall(glColor4us, void, glColor4us_RealName, 0, red, green, blue, alpha)
#define glColor4usv(v) GLEE_GuardedGLCall(glColor4usv, void, glColor4usv_RealName, 0, v)
#define glColorMask(red, green, blue, alpha) GLEE_GuardedGLCall(glColorMask, void, glColorMask_RealName, 0, red, green, blue, alpha)
#define glColorMaski(index, r, g, b, a) GLEE_GuardedGLCall(glColorMaski, void, glColorMaski_RealName, 0, index, r, g, b, a)
#define glColorMaterial(face, mode) GLEE_GuardedGLCall(glColorMaterial, void, glColorMaterial_RealName, 0, face, mode)
#define glColorPointer(size, type, stride, pointer) GLEE_GuardedGLCall(glColorPointer, void, glColorPointer_RealName, 0, size, type, stride, pointer)
#define glCompileShader(shader) GLEE_GuardedGLCall(glCompileShader, void, glCompileShader_RealName, 0, shader)
#define glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data) GLEE_GuardedGLCall(glCompressedTexImage1D, void, glCompressedTexImage1D_RealName, 0, target, level, internalformat, width, border, imageSize, data)
#define glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data) GLEE_GuardedGLCall(glCompressedTexImage2D, void, glCompressedTexImage2D_RealName, 0, target, level, internalformat, width, height, border, imageSize, data)
#define glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data) GLEE_GuardedGLCall(glCompressedTexImage3D, void, glCompressedTexImage3D_RealName, 0, target, level, internalformat, width, height, depth, border, imageSize, data)
#define glCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data) GLEE_GuardedGLCall(glCompressedTexSubImage1D, void, glCompressedTexSubImage1D_RealName, 0, target, level, xoffset, width, format, imageSize, data)
#define glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data) GLEE_GuardedGLCall(glCompressedTexSubImage2D, void, glCompressedTexSubImage2D_RealName, 0, target, level, xoffset, yoffset, width, height, format, imageSize, data)
#define glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data) GLEE_GuardedGLCall(glCompressedTexSubImage3D, void, glCompressedTexSubImage3D_RealName, 0, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data)
#define glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size) GLEE_GuardedGLCall(glCopyBufferSubData, void, glCopyBufferSubData_RealName, 0, readTarget, writeTarget, readOffset, writeOffset, size)
#define glCopyPixels(x, y, width, height, type) GLEE_GuardedGLCall(glCopyPixels, void, glCopyPixels_RealName, 0, x, y, width, height, type)
#define glCopyTexImage1D(target, level, internalFormat, x, y, width, border) GLEE_GuardedGLCall(glCopyTexImage1D, void, glCopyTexImage1D_RealName, 0, target, level, internalFormat, x, y, width, border)
#define glCopyTexImage2D(target, level, internalFormat, x, y, width, height, border) GLEE_GuardedGLCall(glCopyTexImage2D, void, glCopyTexImage2D_RealName, 0, target, level, internalFormat, x, y, width, height, border)
#define glCopyTexSubImage1D(target, level, xoffset, x, y, width) GLEE_GuardedGLCall(glCopyTexSubImage1D, void, glCopyTexSubImage1D_RealName, 0, target, level, xoffset, x, y, width)
#define glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height) GLEE_GuardedGLCall(glCopyTexSubImage2D, void, glCopyTexSubImage2D_RealName, 0, target, level, xoffset, yoffset, x, y, width, height)
#define glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height) GLEE_GuardedGLCall(glCopyTexSubImage3D, void, glCopyTexSubImage3D_RealName, 0, target, level, xoffset, yoffset, zoffset, x, y, width, height)
#define glCreateProgram() GLEE_GuardedGLCallWithReturn(glCreateProgram, GLuint, glCreateProgram_RealName, 0)
#define glCreateShader(type) GLEE_GuardedGLCallWithReturn(glCreateShader, GLuint, glCreateShader_RealName, 0, type)
#define glCreateShaderProgramv(type, count, strings) GLEE_GuardedGLCallWithReturn(glCreateShaderProgramv, GLuint, glCreateShaderProgramv_RealName, 0, type, count, strings)
#define glCullFace(mode) GLEE_GuardedGLCall(glCullFace, void, glCullFace_RealName, 0, mode)
#define glDeleteBuffers(n, buffers) GLEE_GuardedGLCall(glDeleteBuffers, void, glDeleteBuffers_RealName, 0, n, buffers)
#define glDeleteFramebuffers(n, framebuffers) GLEE_GuardedGLCall(glDeleteFramebuffers, void, glDeleteFramebuffers_RealName, 0, n, framebuffers)
#define glDeleteLists(list, range) GLEE_GuardedGLCall(glDeleteLists, void, glDeleteLists_RealName, 0, list, range)
#define glDeleteProgram(program) GLEE_GuardedGLCall(glDeleteProgram, void, glDeleteProgram_RealName, 0, program)
#define glDeleteProgramPipelines(n, pipelines) GLEE_GuardedGLCall(glDeleteProgramPipelines, void, glDeleteProgramPipelines_RealName, 0, n, pipelines)
#define glDeleteQueries(n, ids) GLEE_GuardedGLCall(glDeleteQueries, void, glDeleteQueries_RealName, 0, n, ids)
#define glDeleteRenderbuffers(n, renderbuffers) GLEE_GuardedGLCall(glDeleteRenderbuffers, void, glDeleteRenderbuffers_RealName, 0, n, renderbuffers)
#define glDeleteSamplers(count, samplers) GLEE_GuardedGLCall(glDeleteSamplers, void, glDeleteSamplers_RealName, 0, count, samplers)
#define glDeleteShader(shader) GLEE_GuardedGLCall(glDeleteShader, void, glDeleteShader_RealName, 0, shader)
#define glDeleteSync(sync) GLEE_GuardedGLCall(glDeleteSync, void, glDeleteSync_RealName, 0, sync)
#define glDeleteTextures(n, textures) GLEE_GuardedGLCall(glDeleteTextures, void, glDeleteTextures_RealName, 0, n, textures)
#define glDeleteTransformFeedbacks(n, ids) GLEE_GuardedGLCall(glDeleteTransformFeedbacks, void, glDeleteTransformFeedbacks_RealName, 0, n, ids)
#define glDeleteVertexArrays(n, arrays) GLEE_GuardedGLCall(glDeleteVertexArrays, void, glDeleteVertexArrays_RealName, 0, n, arrays)
#define glDepthFunc(func) GLEE_GuardedGLCall(glDepthFunc, void, glDepthFunc_RealName, 0, func)
#define glDepthMask(flag) GLEE_GuardedGLCall(glDepthMask, void, glDepthMask_RealName, 0, flag)
#define glDepthRange(zNear, zFar) GLEE_GuardedGLCall(glDepthRange, void, glDepthRange_RealName, 0, zNear, zFar)
#define glDepthRangeArrayv(first, count, v) GLEE_GuardedGLCall(glDepthRangeArrayv, void, glDepthRangeArrayv_RealName, 0, first, count, v)
#define glDepthRangeIndexed(index, n, f) GLEE_GuardedGLCall(glDepthRangeIndexed, void, glDepthRangeIndexed_RealName, 0, index, n, f)
#define glDepthRangef(n, f) GLEE_GuardedGLCall(glDepthRangef, void, glDepthRangef_RealName, 0, n, f)
#define glDetachShader(program, shader) GLEE_GuardedGLCall(glDetachShader, void, glDetachShader_RealName, 0, program, shader)
#define glDisable(cap) GLEE_GuardedGLCall(glDisable, void, glDisable_RealName, 0, cap)
#define glDisableClientState(array) GLEE_GuardedGLCall(glDisableClientState, void, glDisableClientState_RealName, 0, array)
#define glDisableVertexAttribArray(index) GLEE_GuardedGLCall(glDisableVertexAttribArray, void, glDisableVertexAttribArray_RealName, 0, index)
#define glDisablei(target, index) GLEE_GuardedGLCall(glDisablei, void, glDisablei_RealName, 0, target, index)
#define glDrawArrays(mode, first, count) GLEE_GuardedGLCall(glDrawArrays, void, glDrawArrays_RealName, 0, mode, first, count)
#define glDrawArraysIndirect(mode, indirect) GLEE_GuardedGLCall(glDrawArraysIndirect, void, glDrawArraysIndirect_RealName, 0, mode, indirect)
#define glDrawArraysInstanced(mode, first, count, instancecount) GLEE_GuardedGLCall(glDrawArraysInstanced, void, glDrawArraysInstanced_RealName, 0, mode, first, count, instancecount)
#define glDrawBuffer(mode) GLEE_GuardedGLCall(glDrawBuffer, void, glDrawBuffer_RealName, 0, mode)
#define glDrawBuffers(n, bufs) GLEE_GuardedGLCall(glDrawBuffers, void, glDrawBuffers_RealName, 0, n, bufs)
#define glDrawElements(mode, count, type, indices) GLEE_GuardedGLCall(glDrawElements, void, glDrawElements_RealName, 0, mode, count, type, indices)
#define glDrawElementsBaseVertex(mode, count, type, indices, basevertex) GLEE_GuardedGLCall(glDrawElementsBaseVertex, void, glDrawElementsBaseVertex_RealName, 0, mode, count, type, indices, basevertex)
#define glDrawElementsIndirect(mode, type, indirect) GLEE_GuardedGLCall(glDrawElementsIndirect, void, glDrawElementsIndirect_RealName, 0, mode, type, indirect)
#define glDrawElementsInstanced(mode, count, type, indices, instancecount) GLEE_GuardedGLCall(glDrawElementsInstanced, void, glDrawElementsInstanced_RealName, 0, mode, count, type, indices, instancecount)
#define glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex) GLEE_GuardedGLCall(glDrawElementsInstancedBaseVertex, void, glDrawElementsInstancedBaseVertex_RealName, 0, mode, count, type, indices, instancecount, basevertex)
#define glDrawPixels(width, height, format, type, pixels) GLEE_GuardedGLCall(glDrawPixels, void, glDrawPixels_RealName, 0, width, height, format, type, pixels)
#define glDrawRangeElements(mode, start, end, count, type, indices) GLEE_GuardedGLCall(glDrawRangeElements, void, glDrawRangeElements_RealName, 0, mode, start, end, count, type, indices)
#define glDrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex) GLEE_GuardedGLCall(glDrawRangeElementsBaseVertex, void, glDrawRangeElementsBaseVertex_RealName, 0, mode, start, end, count, type, indices, basevertex)
#define glDrawTransformFeedback(mode, id) GLEE_GuardedGLCall(glDrawTransformFeedback, void, glDrawTransformFeedback_RealName, 0, mode, id)
#define glDrawTransformFeedbackStream(mode, id, stream) GLEE_GuardedGLCall(glDrawTransformFeedbackStream, void, glDrawTransformFeedbackStream_RealName, 0, mode, id, stream)
#define glEdgeFlag(flag) GLEE_GuardedGLCall(glEdgeFlag, void, glEdgeFlag_RealName, 0, flag)
#define glEdgeFlagPointer(stride, pointer) GLEE_GuardedGLCall(glEdgeFlagPointer, void, glEdgeFlagPointer_RealName, 0, stride, pointer)
#define glEdgeFlagv(flag) GLEE_GuardedGLCall(glEdgeFlagv, void, glEdgeFlagv_RealName, 0, flag)
#define glEnable(cap) GLEE_GuardedGLCall(glEnable, void, glEnable_RealName, 0, cap)
#define glEnableClientState(array) GLEE_GuardedGLCall(glEnableClientState, void, glEnableClientState_RealName, 0, array)
#define glEnableVertexAttribArray(index) GLEE_GuardedGLCall(glEnableVertexAttribArray, void, glEnableVertexAttribArray_RealName, 0, index)
#define glEnablei(target, index) GLEE_GuardedGLCall(glEnablei, void, glEnablei_RealName, 0, target, index)
#define glEnd() GLEE_GuardedGLCall(glEnd, void, glEnd_RealName, 0)
#define glEndConditionalRender() GLEE_GuardedGLCall(glEndConditionalRender, void, glEndConditionalRender_RealName, 0)
#define glEndList() GLEE_GuardedGLCall(glEndList, void, glEndList_RealName, 0)
#define glEndQuery(target) GLEE_GuardedGLCall(glEndQuery, void, glEndQuery_RealName, 0, target)
#define glEndQueryIndexed(target, index) GLEE_GuardedGLCall(glEndQueryIndexed, void, glEndQueryIndexed_RealName, 0, target, index)
#define glEndTransformFeedback() GLEE_GuardedGLCall(glEndTransformFeedback, void, glEndTransformFeedback_RealName, 0)
#define glEvalCoord1d(u) GLEE_GuardedGLCall(glEvalCoord1d, void, glEvalCoord1d_RealName, 0, u)
#define glEvalCoord1dv(u) GLEE_GuardedGLCall(glEvalCoord1dv, void, glEvalCoord1dv_RealName, 0, u)
#define glEvalCoord1f(u) GLEE_GuardedGLCall(glEvalCoord1f, void, glEvalCoord1f_RealName, 0, u)
#define glEvalCoord1fv(u) GLEE_GuardedGLCall(glEvalCoord1fv, void, glEvalCoord1fv_RealName, 0, u)
#define glEvalCoord2d(u, v) GLEE_GuardedGLCall(glEvalCoord2d, void, glEvalCoord2d_RealName, 0, u, v)
#define glEvalCoord2dv(u) GLEE_GuardedGLCall(glEvalCoord2dv, void, glEvalCoord2dv_RealName, 0, u)
#define glEvalCoord2f(u, v) GLEE_GuardedGLCall(glEvalCoord2f, void, glEvalCoord2f_RealName, 0, u, v)
#define glEvalCoord2fv(u) GLEE_GuardedGLCall(glEvalCoord2fv, void, glEvalCoord2fv_RealName, 0, u)
#define glEvalMesh1(mode, i1, i2) GLEE_GuardedGLCall(glEvalMesh1, void, glEvalMesh1_RealName, 0, mode, i1, i2)
#define glEvalMesh2(mode, i1, i2, j1, j2) GLEE_GuardedGLCall(glEvalMesh2, void, glEvalMesh2_RealName, 0, mode, i1, i2, j1, j2)
#define glEvalPoint1(i) GLEE_GuardedGLCall(glEvalPoint1, void, glEvalPoint1_RealName, 0, i)
#define glEvalPoint2(i, j) GLEE_GuardedGLCall(glEvalPoint2, void, glEvalPoint2_RealName, 0, i, j)
#define glFeedbackBuffer(size, type, buffer) GLEE_GuardedGLCall(glFeedbackBuffer, void, glFeedbackBuffer_RealName, 0, size, type, buffer)
#define glFenceSync(condition, flags) GLEE_GuardedGLCallWithReturn(glFenceSync, GLsync, glFenceSync_RealName, 0, condition, flags)
#define glFinish() GLEE_GuardedGLCall(glFinish, void, glFinish_RealName, 0)
#define glFlush() GLEE_GuardedGLCall(glFlush, void, glFlush_RealName, 0)
#define glFlushMappedBufferRange(target, offset, length) GLEE_GuardedGLCall(glFlushMappedBufferRange, void, glFlushMappedBufferRange_RealName, 0, target, offset, length)
#define glFogf(pname, param) GLEE_GuardedGLCall(glFogf, void, glFogf_RealName, 0, pname, param)
#define glFogfv(pname, params) GLEE_GuardedGLCall(glFogfv, void, glFogfv_RealName, 0, pname, params)
#define glFogi(pname, param) GLEE_GuardedGLCall(glFogi, void, glFogi_RealName, 0, pname, param)
#define glFogiv(pname, params) GLEE_GuardedGLCall(glFogiv, void, glFogiv_RealName, 0, pname, params)
#define glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer) GLEE_GuardedGLCall(glFramebufferRenderbuffer, void, glFramebufferRenderbuffer_RealName, 0, target, attachment, renderbuffertarget, renderbuffer)
#define glFramebufferTexture(target, attachment, texture, level) GLEE_GuardedGLCall(glFramebufferTexture, void, glFramebufferTexture_RealName, 0, target, attachment, texture, level)
#define glFramebufferTexture1D(target, attachment, textarget, texture, level) GLEE_GuardedGLCall(glFramebufferTexture1D, void, glFramebufferTexture1D_RealName, 0, target, attachment, textarget, texture, level)
#define glFramebufferTexture2D(target, attachment, textarget, texture, level) GLEE_GuardedGLCall(glFramebufferTexture2D, void, glFramebufferTexture2D_RealName, 0, target, attachment, textarget, texture, level)
#define glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset) GLEE_GuardedGLCall(glFramebufferTexture3D, void, glFramebufferTexture3D_RealName, 0, target, attachment, textarget, texture, level, zoffset)
#define glFramebufferTextureLayer(target, attachment, texture, level, layer) GLEE_GuardedGLCall(glFramebufferTextureLayer, void, glFramebufferTextureLayer_RealName, 0, target, attachment, texture, level, layer)
#define glFrontFace(mode) GLEE_GuardedGLCall(glFrontFace, void, glFrontFace_RealName, 0, mode)
#define glFrustum(left, right, bottom, top, zNear, zFar) GLEE_GuardedGLCall(glFrustum, void, glFrustum_RealName, 0, left, right, bottom, top, zNear, zFar)
#define glGenBuffers(n, buffers) GLEE_GuardedGLCall(glGenBuffers, void, glGenBuffers_RealName, 0, n, buffers)
#define glGenFramebuffers(n, framebuffers) GLEE_GuardedGLCall(glGenFramebuffers, void, glGenFramebuffers_RealName, 0, n, framebuffers)
#define glGenLists(range) GLEE_GuardedGLCallWithReturn(glGenLists, GLuint, glGenLists_RealName, 0, range)
#define glGenProgramPipelines(n, pipelines) GLEE_GuardedGLCall(glGenProgramPipelines, void, glGenProgramPipelines_RealName, 0, n, pipelines)
#define glGenQueries(n, ids) GLEE_GuardedGLCall(glGenQueries, void, glGenQueries_RealName, 0, n, ids)
#define glGenRenderbuffers(n, renderbuffers) GLEE_GuardedGLCall(glGenRenderbuffers, void, glGenRenderbuffers_RealName, 0, n, renderbuffers)
#define glGenSamplers(count, samplers) GLEE_GuardedGLCall(glGenSamplers, void, glGenSamplers_RealName, 0, count, samplers)
#define glGenTextures(n, textures) GLEE_GuardedGLCall(glGenTextures, void, glGenTextures_RealName, 0, n, textures)
#define glGenTransformFeedbacks(n, ids) GLEE_GuardedGLCall(glGenTransformFeedbacks, void, glGenTransformFeedbacks_RealName, 0, n, ids)
#define glGenVertexArrays(n, arrays) GLEE_GuardedGLCall(glGenVertexArrays, void, glGenVertexArrays_RealName, 0, n, arrays)
#define glGenerateMipmap(target) GLEE_GuardedGLCall(glGenerateMipmap, void, glGenerateMipmap_RealName, 0, target)
#define glGetActiveAttrib(program, index, bufSize, length, size, type, name) GLEE_GuardedGLCall(glGetActiveAttrib, void, glGetActiveAttrib_RealName, 0, program, index, bufSize, length, size, type, name)
#define glGetActiveSubroutineName(program, shadertype, index, bufsize, length, name) GLEE_GuardedGLCall(glGetActiveSubroutineName, void, glGetActiveSubroutineName_RealName, 0, program, shadertype, index, bufsize, length, name)
#define glGetActiveSubroutineUniformName(program, shadertype, index, bufsize, length, name) GLEE_GuardedGLCall(glGetActiveSubroutineUniformName, void, glGetActiveSubroutineUniformName_RealName, 0, program, shadertype, index, bufsize, length, name)
#define glGetActiveSubroutineUniformiv(program, shadertype, index, pname, values) GLEE_GuardedGLCall(glGetActiveSubroutineUniformiv, void, glGetActiveSubroutineUniformiv_RealName, 0, program, shadertype, index, pname, values)
#define glGetActiveUniform(program, index, bufSize, length, size, type, name) GLEE_GuardedGLCall(glGetActiveUniform, void, glGetActiveUniform_RealName, 0, program, index, bufSize, length, size, type, name)
#define glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName) GLEE_GuardedGLCall(glGetActiveUniformBlockName, void, glGetActiveUniformBlockName_RealName, 0, program, uniformBlockIndex, bufSize, length, uniformBlockName)
#define glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params) GLEE_GuardedGLCall(glGetActiveUniformBlockiv, void, glGetActiveUniformBlockiv_RealName, 0, program, uniformBlockIndex, pname, params)
#define glGetActiveUniformName(program, uniformIndex, bufSize, length, uniformName) GLEE_GuardedGLCall(glGetActiveUniformName, void, glGetActiveUniformName_RealName, 0, program, uniformIndex, bufSize, length, uniformName)
#define glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params) GLEE_GuardedGLCall(glGetActiveUniformsiv, void, glGetActiveUniformsiv_RealName, 0, program, uniformCount, uniformIndices, pname, params)
#define glGetAttachedShaders(program, maxCount, count, shaders) GLEE_GuardedGLCall(glGetAttachedShaders, void, glGetAttachedShaders_RealName, 0, program, maxCount, count, shaders)
#define glGetAttribLocation(program, name) GLEE_GuardedGLCallWithReturn(glGetAttribLocation, GLint, glGetAttribLocation_RealName, 0, program, name)
#define glGetBooleani_v(target, index, data) GLEE_GuardedGLCall(glGetBooleani_v, void, glGetBooleani_v_RealName, 0, target, index, data)
#define glGetBooleanv(pname, params) GLEE_GuardedGLCall(glGetBooleanv, void, glGetBooleanv_RealName, 0, pname, params)
#define glGetBufferParameteri64v(target, pname, params) GLEE_GuardedGLCall(glGetBufferParameteri64v, void, glGetBufferParameteri64v_RealName, 0, target, pname, params)
#define glGetBufferParameteriv(target, pname, params) GLEE_GuardedGLCall(glGetBufferParameteriv, void, glGetBufferParameteriv_RealName, 0, target, pname, params)
#define glGetBufferPointerv(target, pname, params) GLEE_GuardedGLCall(glGetBufferPointerv, void, glGetBufferPointerv_RealName, 0, target, pname, params)
#define glGetBufferSubData(target, offset, size, data) GLEE_GuardedGLCall(glGetBufferSubData, void, glGetBufferSubData_RealName, 0, target, offset, size, data)
#define glGetClipPlane(plane, equation) GLEE_GuardedGLCall(glGetClipPlane, void, glGetClipPlane_RealName, 0, plane, equation)
#define glGetCompressedTexImage(target, level, img) GLEE_GuardedGLCall(glGetCompressedTexImage, void, glGetCompressedTexImage_RealName, 0, target, level, img)
#define glGetDoublei_v(target, index, data) GLEE_GuardedGLCall(glGetDoublei_v, void, glGetDoublei_v_RealName, 0, target, index, data)
#define glGetDoublev(pname, params) GLEE_GuardedGLCall(glGetDoublev, void, glGetDoublev_RealName, 0, pname, params)
#define glGetError glGetError_RealName
#define glGetFloati_v(target, index, data) GLEE_GuardedGLCall(glGetFloati_v, void, glGetFloati_v_RealName, 0, target, index, data)
#define glGetFloatv(pname, params) GLEE_GuardedGLCall(glGetFloatv, void, glGetFloatv_RealName, 0, pname, params)
#define glGetFragDataIndex(program, name) GLEE_GuardedGLCallWithReturn(glGetFragDataIndex, GLint, glGetFragDataIndex_RealName, 0, program, name)
#define glGetFragDataLocation(program, name) GLEE_GuardedGLCallWithReturn(glGetFragDataLocation, GLint, glGetFragDataLocation_RealName, 0, program, name)
#define glGetFramebufferAttachmentParameteriv(target, attachment, pname, params) GLEE_GuardedGLCall(glGetFramebufferAttachmentParameteriv, void, glGetFramebufferAttachmentParameteriv_RealName, 0, target, attachment, pname, params)
#define glGetInteger64i_v(target, index, data) GLEE_GuardedGLCall(glGetInteger64i_v, void, glGetInteger64i_v_RealName, 0, target, index, data)
#define glGetInteger64v(pname, params) GLEE_GuardedGLCall(glGetInteger64v, void, glGetInteger64v_RealName, 0, pname, params)
#define glGetIntegeri_v(target, index, data) GLEE_GuardedGLCall(glGetIntegeri_v, void, glGetIntegeri_v_RealName, 0, target, index, data)
#define glGetIntegerv(pname, params) GLEE_GuardedGLCall(glGetIntegerv, void, glGetIntegerv_RealName, 0, pname, params)
#define glGetLightfv(light, pname, params) GLEE_GuardedGLCall(glGetLightfv, void, glGetLightfv_RealName, 0, light, pname, params)
#define glGetLightiv(light, pname, params) GLEE_GuardedGLCall(glGetLightiv, void, glGetLightiv_RealName, 0, light, pname, params)
#define glGetMapdv(target, query, v) GLEE_GuardedGLCall(glGetMapdv, void, glGetMapdv_RealName, 0, target, query, v)
#define glGetMapfv(target, query, v) GLEE_GuardedGLCall(glGetMapfv, void, glGetMapfv_RealName, 0, target, query, v)
#define glGetMapiv(target, query, v) GLEE_GuardedGLCall(glGetMapiv, void, glGetMapiv_RealName, 0, target, query, v)
#define glGetMaterialfv(face, pname, params) GLEE_GuardedGLCall(glGetMaterialfv, void, glGetMaterialfv_RealName, 0, face, pname, params)
#define glGetMaterialiv(face, pname, params) GLEE_GuardedGLCall(glGetMaterialiv, void, glGetMaterialiv_RealName, 0, face, pname, params)
#define glGetMultisamplefv(pname, index, val) GLEE_GuardedGLCall(glGetMultisamplefv, void, glGetMultisamplefv_RealName, 0, pname, index, val)
#define glGetPixelMapfv(map, values) GLEE_GuardedGLCall(glGetPixelMapfv, void, glGetPixelMapfv_RealName, 0, map, values)
#define glGetPixelMapuiv(map, values) GLEE_GuardedGLCall(glGetPixelMapuiv, void, glGetPixelMapuiv_RealName, 0, map, values)
#define glGetPixelMapusv(map, values) GLEE_GuardedGLCall(glGetPixelMapusv, void, glGetPixelMapusv_RealName, 0, map, values)
#define glGetPointerv(pname, params) GLEE_GuardedGLCall(glGetPointerv, void, glGetPointerv_RealName, 0, pname, params)
#define glGetPolygonStipple(mask) GLEE_GuardedGLCall(glGetPolygonStipple, void, glGetPolygonStipple_RealName, 0, mask)
#define glGetProgramBinary(program, bufSize, length, binaryFormat, binary) GLEE_GuardedGLCall(glGetProgramBinary, void, glGetProgramBinary_RealName, 0, program, bufSize, length, binaryFormat, binary)
#define glGetProgramInfoLog(program, bufSize, length, infoLog) GLEE_GuardedGLCall(glGetProgramInfoLog, void, glGetProgramInfoLog_RealName, 0, program, bufSize, length, infoLog)
#define glGetProgramPipelineInfoLog(pipeline, bufSize, length, infoLog) GLEE_GuardedGLCall(glGetProgramPipelineInfoLog, void, glGetProgramPipelineInfoLog_RealName, 0, pipeline, bufSize, length, infoLog)
#define glGetProgramPipelineiv(pipeline, pname, params) GLEE_GuardedGLCall(glGetProgramPipelineiv, void, glGetProgramPipelineiv_RealName, 0, pipeline, pname, params)
#define glGetProgramStageiv(program, shadertype, pname, values) GLEE_GuardedGLCall(glGetProgramStageiv, void, glGetProgramStageiv_RealName, 0, program, shadertype, pname, values)
#define glGetProgramiv(program, pname, params) GLEE_GuardedGLCall(glGetProgramiv, void, glGetProgramiv_RealName, 0, program, pname, params)
#define glGetQueryIndexediv(target, index, pname, params) GLEE_GuardedGLCall(glGetQueryIndexediv, void, glGetQueryIndexediv_RealName, 0, target, index, pname, params)
#define glGetQueryObjecti64v(id, pname, params) GLEE_GuardedGLCall(glGetQueryObjecti64v, void, glGetQueryObjecti64v_RealName, 0, id, pname, params)
#define glGetQueryObjectiv(id, pname, params) GLEE_GuardedGLCall(glGetQueryObjectiv, void, glGetQueryObjectiv_RealName, 0, id, pname, params)
#define glGetQueryObjectui64v(id, pname, params) GLEE_GuardedGLCall(glGetQueryObjectui64v, void, glGetQueryObjectui64v_RealName, 0, id, pname, params)
#define glGetQueryObjectuiv(id, pname, params) GLEE_GuardedGLCall(glGetQueryObjectuiv, void, glGetQueryObjectuiv_RealName, 0, id, pname, params)
#define glGetQueryiv(target, pname, params) GLEE_GuardedGLCall(glGetQueryiv, void, glGetQueryiv_RealName, 0, target, pname, params)
#define glGetRenderbufferParameteriv(target, pname, params) GLEE_GuardedGLCall(glGetRenderbufferParameteriv, void, glGetRenderbufferParameteriv_RealName, 0, target, pname, params)
#define glGetSamplerParameterIiv(sampler, pname, params) GLEE_GuardedGLCall(glGetSamplerParameterIiv, void, glGetSamplerParameterIiv_RealName, 0, sampler, pname, params)
#define glGetSamplerParameterIuiv(sampler, pname, params) GLEE_GuardedGLCall(glGetSamplerParameterIuiv, void, glGetSamplerParameterIuiv_RealName, 0, sampler, pname, params)
#define glGetSamplerParameterfv(sampler, pname, params) GLEE_GuardedGLCall(glGetSamplerParameterfv, void, glGetSamplerParameterfv_RealName, 0, sampler, pname, params)
#define glGetSamplerParameteriv(sampler, pname, params) GLEE_GuardedGLCall(glGetSamplerParameteriv, void, glGetSamplerParameteriv_RealName, 0, sampler, pname, params)
#define glGetShaderInfoLog(shader, bufSize, length, infoLog) GLEE_GuardedGLCall(glGetShaderInfoLog, void, glGetShaderInfoLog_RealName, 0, shader, bufSize, length, infoLog)
#define glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision) GLEE_GuardedGLCall(glGetShaderPrecisionFormat, void, glGetShaderPrecisionFormat_RealName, 0, shadertype, precisiontype, range, precision)
#define glGetShaderSource(shader, bufSize, length, source) GLEE_GuardedGLCall(glGetShaderSource, void, glGetShaderSource_RealName, 0, shader, bufSize, length, source)
#define glGetShaderiv(shader, pname, params) GLEE_GuardedGLCall(glGetShaderiv, void, glGetShaderiv_RealName, 0, shader, pname, params)
#define glGetString(name) GLEE_GuardedGLCallWithReturn(glGetString, const GLubyte *, glGetString_RealName, 0, name)
#define glGetStringi(name, index) GLEE_GuardedGLCallWithReturn(glGetStringi, const GLubyte *, glGetStringi_RealName, 0, name, index)
#define glGetSubroutineIndex(program, shadertype, name) GLEE_GuardedGLCallWithReturn(glGetSubroutineIndex, GLuint, glGetSubroutineIndex_RealName, 0, program, shadertype, name)
#define glGetSubroutineUniformLocation(program, shadertype, name) GLEE_GuardedGLCallWithReturn(glGetSubroutineUniformLocation, GLint, glGetSubroutineUniformLocation_RealName, 0, program, shadertype, name)
#define glGetSynciv(sync, pname, bufSize, length, values) GLEE_GuardedGLCall(glGetSynciv, void, glGetSynciv_RealName, 0, sync, pname, bufSize, length, values)
#define glGetTexEnvfv(target, pname, params) GLEE_GuardedGLCall(glGetTexEnvfv, void, glGetTexEnvfv_RealName, 0, target, pname, params)
#define glGetTexEnviv(target, pname, params) GLEE_GuardedGLCall(glGetTexEnviv, void, glGetTexEnviv_RealName, 0, target, pname, params)
#define glGetTexGendv(coord, pname, params) GLEE_GuardedGLCall(glGetTexGendv, void, glGetTexGendv_RealName, 0, coord, pname, params)
#define glGetTexGenfv(coord, pname, params) GLEE_GuardedGLCall(glGetTexGenfv, void, glGetTexGenfv_RealName, 0, coord, pname, params)
#define glGetTexGeniv(coord, pname, params) GLEE_GuardedGLCall(glGetTexGeniv, void, glGetTexGeniv_RealName, 0, coord, pname, params)
#define glGetTexImage(target, level, format, type, pixels) GLEE_GuardedGLCall(glGetTexImage, void, glGetTexImage_RealName, 0, target, level, format, type, pixels)
#define glGetTexLevelParameterfv(target, level, pname, params) GLEE_GuardedGLCall(glGetTexLevelParameterfv, void, glGetTexLevelParameterfv_RealName, 0, target, level, pname, params)
#define glGetTexLevelParameteriv(target, level, pname, params) GLEE_GuardedGLCall(glGetTexLevelParameteriv, void, glGetTexLevelParameteriv_RealName, 0, target, level, pname, params)
#define glGetTexParameterIiv(target, pname, params) GLEE_GuardedGLCall(glGetTexParameterIiv, void, glGetTexParameterIiv_RealName, 0, target, pname, params)
#define glGetTexParameterIuiv(target, pname, params) GLEE_GuardedGLCall(glGetTexParameterIuiv, void, glGetTexParameterIuiv_RealName, 0, target, pname, params)
#define glGetTexParameterfv(target, pname, params) GLEE_GuardedGLCall(glGetTexParameterfv, void, glGetTexParameterfv_RealName, 0, target, pname, params)
#define glGetTexParameteriv(target, pname, params) GLEE_GuardedGLCall(glGetTexParameteriv, void, glGetTexParameteriv_RealName, 0, target, pname, params)
#define glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name) GLEE_GuardedGLCall(glGetTransformFeedbackVarying, void, glGetTransformFeedbackVarying_RealName, 0, program, index, bufSize, length, size, type, name)
#define glGetUniformBlockIndex(program, uniformBlockName) GLEE_GuardedGLCallWithReturn(glGetUniformBlockIndex, GLuint, glGetUniformBlockIndex_RealName, 0, program, uniformBlockName)
#define glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices) GLEE_GuardedGLCall(glGetUniformIndices, void, glGetUniformIndices_RealName, 0, program, uniformCount, uniformNames, uniformIndices)
#define glGetUniformLocation(program, name) GLEE_GuardedGLCallWithReturn(glGetUniformLocation, GLint, glGetUniformLocation_RealName, 0, program, name)
#define glGetUniformSubroutineuiv(shadertype, location, params) GLEE_GuardedGLCall(glGetUniformSubroutineuiv, void, glGetUniformSubroutineuiv_RealName, 0, shadertype, location, params)
#define glGetUniformdv(program, location, params) GLEE_GuardedGLCall(glGetUniformdv, void, glGetUniformdv_RealName, 0, program, location, params)
#define glGetUniformfv(program, location, params) GLEE_GuardedGLCall(glGetUniformfv, void, glGetUniformfv_RealName, 0, program, location, params)
#define glGetUniformiv(program, location, params) GLEE_GuardedGLCall(glGetUniformiv, void, glGetUniformiv_RealName, 0, program, location, params)
#define glGetUniformuiv(program, location, params) GLEE_GuardedGLCall(glGetUniformuiv, void, glGetUniformuiv_RealName, 0, program, location, params)
#define glGetVertexAttribIiv(index, pname, params) GLEE_GuardedGLCall(glGetVertexAttribIiv, void, glGetVertexAttribIiv_RealName, 0, index, pname, params)
#define glGetVertexAttribIuiv(index, pname, params) GLEE_GuardedGLCall(glGetVertexAttribIuiv, void, glGetVertexAttribIuiv_RealName, 0, index, pname, params)
#define glGetVertexAttribLdv(index, pname, params) GLEE_GuardedGLCall(glGetVertexAttribLdv, void, glGetVertexAttribLdv_RealName, 0, index, pname, params)
#define glGetVertexAttribPointerv(index, pname, pointer) GLEE_GuardedGLCall(glGetVertexAttribPointerv, void, glGetVertexAttribPointerv_RealName, 0, index, pname, pointer)
#define glGetVertexAttribdv(index, pname, params) GLEE_GuardedGLCall(glGetVertexAttribdv, void, glGetVertexAttribdv_RealName, 0, index, pname, params)
#define glGetVertexAttribfv(index, pname, params) GLEE_GuardedGLCall(glGetVertexAttribfv, void, glGetVertexAttribfv_RealName, 0, index, pname, params)
#define glGetVertexAttribiv(index, pname, params) GLEE_GuardedGLCall(glGetVertexAttribiv, void, glGetVertexAttribiv_RealName, 0, index, pname, params)
#define glHint(target, mode) GLEE_GuardedGLCall(glHint, void, glHint_RealName, 0, target, mode)
#define glIndexMask(mask) GLEE_GuardedGLCall(glIndexMask, void, glIndexMask_RealName, 0, mask)
#define glIndexPointer(type, stride, pointer) GLEE_GuardedGLCall(glIndexPointer, void, glIndexPointer_RealName, 0, type, stride, pointer)
#define glIndexd(c) GLEE_GuardedGLCall(glIndexd, void, glIndexd_RealName, 0, c)
#define glIndexdv(c) GLEE_GuardedGLCall(glIndexdv, void, glIndexdv_RealName, 0, c)
#define glIndexf(c) GLEE_GuardedGLCall(glIndexf, void, glIndexf_RealName, 0, c)
#define glIndexfv(c) GLEE_GuardedGLCall(glIndexfv, void, glIndexfv_RealName, 0, c)
#define glIndexi(c) GLEE_GuardedGLCall(glIndexi, void, glIndexi_RealName, 0, c)
#define glIndexiv(c) GLEE_GuardedGLCall(glIndexiv, void, glIndexiv_RealName, 0, c)
#define glIndexs(c) GLEE_GuardedGLCall(glIndexs, void, glIndexs_RealName, 0, c)
#define glIndexsv(c) GLEE_GuardedGLCall(glIndexsv, void, glIndexsv_RealName, 0, c)
#define glIndexub(c) GLEE_GuardedGLCall(glIndexub, void, glIndexub_RealName, 0, c)
#define glIndexubv(c) GLEE_GuardedGLCall(glIndexubv, void, glIndexubv_RealName, 0, c)
#define glInitNames() GLEE_GuardedGLCall(glInitNames, void, glInitNames_RealName, 0)
#define glInterleavedArrays(format, stride, pointer) GLEE_GuardedGLCall(glInterleavedArrays, void, glInterleavedArrays_RealName, 0, format, stride, pointer)
#define glIsBuffer(buffer) GLEE_GuardedGLCallWithReturn(glIsBuffer, GLboolean, glIsBuffer_RealName, 0, buffer)
#define glIsEnabled(cap) GLEE_GuardedGLCallWithReturn(glIsEnabled, GLboolean, glIsEnabled_RealName, 0, cap)
#define glIsEnabledi(target, index) GLEE_GuardedGLCallWithReturn(glIsEnabledi, GLboolean, glIsEnabledi_RealName, 0, target, index)
#define glIsFramebuffer(framebuffer) GLEE_GuardedGLCallWithReturn(glIsFramebuffer, GLboolean, glIsFramebuffer_RealName, 0, framebuffer)
#define glIsList(list) GLEE_GuardedGLCallWithReturn(glIsList, GLboolean, glIsList_RealName, 0, list)
#define glIsProgram(program) GLEE_GuardedGLCallWithReturn(glIsProgram, GLboolean, glIsProgram_RealName, 0, program)
#define glIsProgramPipeline(pipeline) GLEE_GuardedGLCallWithReturn(glIsProgramPipeline, GLboolean, glIsProgramPipeline_RealName, 0, pipeline)
#define glIsQuery(id) GLEE_GuardedGLCallWithReturn(glIsQuery, GLboolean, glIsQuery_RealName, 0, id)
#define glIsRenderbuffer(renderbuffer) GLEE_GuardedGLCallWithReturn(glIsRenderbuffer, GLboolean, glIsRenderbuffer_RealName, 0, renderbuffer)
#define glIsSampler(sampler) GLEE_GuardedGLCallWithReturn(glIsSampler, GLboolean, glIsSampler_RealName, 0, sampler)
#define glIsShader(shader) GLEE_GuardedGLCallWithReturn(glIsShader, GLboolean, glIsShader_RealName, 0, shader)
#define glIsSync(sync) GLEE_GuardedGLCallWithReturn(glIsSync, GLboolean, glIsSync_RealName, 0, sync)
#define glIsTexture(texture) GLEE_GuardedGLCallWithReturn(glIsTexture, GLboolean, glIsTexture_RealName, 0, texture)
#define glIsTransformFeedback(id) GLEE_GuardedGLCallWithReturn(glIsTransformFeedback, GLboolean, glIsTransformFeedback_RealName, 0, id)
#define glIsVertexArray(array) GLEE_GuardedGLCallWithReturn(glIsVertexArray, GLboolean, glIsVertexArray_RealName, 0, array)
#define glLightModelf(pname, param) GLEE_GuardedGLCall(glLightModelf, void, glLightModelf_RealName, 0, pname, param)
#define glLightModelfv(pname, params) GLEE_GuardedGLCall(glLightModelfv, void, glLightModelfv_RealName, 0, pname, params)
#define glLightModeli(pname, param) GLEE_GuardedGLCall(glLightModeli, void, glLightModeli_RealName, 0, pname, param)
#define glLightModeliv(pname, params) GLEE_GuardedGLCall(glLightModeliv, void, glLightModeliv_RealName, 0, pname, params)
#define glLightf(light, pname, param) GLEE_GuardedGLCall(glLightf, void, glLightf_RealName, 0, light, pname, param)
#define glLightfv(light, pname, params) GLEE_GuardedGLCall(glLightfv, void, glLightfv_RealName, 0, light, pname, params)
#define glLighti(light, pname, param) GLEE_GuardedGLCall(glLighti, void, glLighti_RealName, 0, light, pname, param)
#define glLightiv(light, pname, params) GLEE_GuardedGLCall(glLightiv, void, glLightiv_RealName, 0, light, pname, params)
#define glLineStipple(factor, pattern) GLEE_GuardedGLCall(glLineStipple, void, glLineStipple_RealName, 0, factor, pattern)
#define glLineWidth(width) GLEE_GuardedGLCall(glLineWidth, void, glLineWidth_RealName, 0, width)
#define glLinkProgram(program) GLEE_GuardedGLCall(glLinkProgram, void, glLinkProgram_RealName, 0, program)
#define glListBase(base) GLEE_GuardedGLCall(glListBase, void, glListBase_RealName, 0, base)
#define glLoadIdentity() GLEE_GuardedGLCall(glLoadIdentity, void, glLoadIdentity_RealName, 0)
#define glLoadMatrixd(m) GLEE_GuardedGLCall(glLoadMatrixd, void, glLoadMatrixd_RealName, 0, m)
#define glLoadMatrixf(m) GLEE_GuardedGLCall(glLoadMatrixf, void, glLoadMatrixf_RealName, 0, m)
#define glLoadName(name) GLEE_GuardedGLCall(glLoadName, void, glLoadName_RealName, 0, name)
#define glLogicOp(opcode) GLEE_GuardedGLCall(glLogicOp, void, glLogicOp_RealName, 0, opcode)
#define glMap1d(target, u1, u2, stride, order, points) GLEE_GuardedGLCall(glMap1d, void, glMap1d_RealName, 0, target, u1, u2, stride, order, points)
#define glMap1f(target, u1, u2, stride, order, points) GLEE_GuardedGLCall(glMap1f, void, glMap1f_RealName, 0, target, u1, u2, stride, order, points)
#define glMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points) GLEE_GuardedGLCall(glMap2d, void, glMap2d_RealName, 0, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points)
#define glMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points) GLEE_GuardedGLCall(glMap2f, void, glMap2f_RealName, 0, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points)
#define glMapBuffer(target, access) GLEE_GuardedGLCallWithReturn(glMapBuffer, GLvoid*, glMapBuffer_RealName, 0, target, access)
#define glMapBufferRange(target, offset, length, access) GLEE_GuardedGLCallWithReturn(glMapBufferRange, GLvoid*, glMapBufferRange_RealName, 0, target, offset, length, access)
#define glMapGrid1d(un, u1, u2) GLEE_GuardedGLCall(glMapGrid1d, void, glMapGrid1d_RealName, 0, un, u1, u2)
#define glMapGrid1f(un, u1, u2) GLEE_GuardedGLCall(glMapGrid1f, void, glMapGrid1f_RealName, 0, un, u1, u2)
#define glMapGrid2d(un, u1, u2, vn, v1, v2) GLEE_GuardedGLCall(glMapGrid2d, void, glMapGrid2d_RealName, 0, un, u1, u2, vn, v1, v2)
#define glMapGrid2f(un, u1, u2, vn, v1, v2) GLEE_GuardedGLCall(glMapGrid2f, void, glMapGrid2f_RealName, 0, un, u1, u2, vn, v1, v2)
#define glMaterialf(face, pname, param) GLEE_GuardedGLCall(glMaterialf, void, glMaterialf_RealName, 0, face, pname, param)
#define glMaterialfv(face, pname, params) GLEE_GuardedGLCall(glMaterialfv, void, glMaterialfv_RealName, 0, face, pname, params)
#define glMateriali(face, pname, param) GLEE_GuardedGLCall(glMateriali, void, glMateriali_RealName, 0, face, pname, param)
#define glMaterialiv(face, pname, params) GLEE_GuardedGLCall(glMaterialiv, void, glMaterialiv_RealName, 0, face, pname, params)
#define glMatrixMode(mode) GLEE_GuardedGLCall(glMatrixMode, void, glMatrixMode_RealName, 0, mode)
#define glMinSampleShading(value) GLEE_GuardedGLCall(glMinSampleShading, void, glMinSampleShading_RealName, 0, value)
#define glMultMatrixd(m) GLEE_GuardedGLCall(glMultMatrixd, void, glMultMatrixd_RealName, 0, m)
#define glMultMatrixf(m) GLEE_GuardedGLCall(glMultMatrixf, void, glMultMatrixf_RealName, 0, m)
#define glMultiDrawArrays(mode, first, count, drawcount) GLEE_GuardedGLCall(glMultiDrawArrays, void, glMultiDrawArrays_RealName, 0, mode, first, count, drawcount)
#define glMultiDrawElements(mode, count, type, indices, drawcount) GLEE_GuardedGLCall(glMultiDrawElements, void, glMultiDrawElements_RealName, 0, mode, count, type, indices, drawcount)
#define glMultiDrawElementsBaseVertex(mode, count, type, indices, drawcount, basevertex) GLEE_GuardedGLCall(glMultiDrawElementsBaseVertex, void, glMultiDrawElementsBaseVertex_RealName, 0, mode, count, type, indices, drawcount, basevertex)
#define glNewList(list, mode) GLEE_GuardedGLCall(glNewList, void, glNewList_RealName, 0, list, mode)
#define glNormal3b(nx, ny, nz) GLEE_GuardedGLCall(glNormal3b, void, glNormal3b_RealName, 0, nx, ny, nz)
#define glNormal3bv(v) GLEE_GuardedGLCall(glNormal3bv, void, glNormal3bv_RealName, 0, v)
#define glNormal3d(nx, ny, nz) GLEE_GuardedGLCall(glNormal3d, void, glNormal3d_RealName, 0, nx, ny, nz)
#define glNormal3dv(v) GLEE_GuardedGLCall(glNormal3dv, void, glNormal3dv_RealName, 0, v)
#define glNormal3f(nx, ny, nz) GLEE_GuardedGLCall(glNormal3f, void, glNormal3f_RealName, 0, nx, ny, nz)
#define glNormal3fv(v) GLEE_GuardedGLCall(glNormal3fv, void, glNormal3fv_RealName, 0, v)
#define glNormal3i(nx, ny, nz) GLEE_GuardedGLCall(glNormal3i, void, glNormal3i_RealName, 0, nx, ny, nz)
#define glNormal3iv(v) GLEE_GuardedGLCall(glNormal3iv, void, glNormal3iv_RealName, 0, v)
#define glNormal3s(nx, ny, nz) GLEE_GuardedGLCall(glNormal3s, void, glNormal3s_RealName, 0, nx, ny, nz)
#define glNormal3sv(v) GLEE_GuardedGLCall(glNormal3sv, void, glNormal3sv_RealName, 0, v)
#define glNormalPointer(type, stride, pointer) GLEE_GuardedGLCall(glNormalPointer, void, glNormalPointer_RealName, 0, type, stride, pointer)
#define glOrtho(left, right, bottom, top, zNear, zFar) GLEE_GuardedGLCall(glOrtho, void, glOrtho_RealName, 0, left, right, bottom, top, zNear, zFar)
#define glPassThrough(token) GLEE_GuardedGLCall(glPassThrough, void, glPassThrough_RealName, 0, token)
#define glPatchParameterfv(pname, values) GLEE_GuardedGLCall(glPatchParameterfv, void, glPatchParameterfv_RealName, 0, pname, values)
#define glPatchParameteri(pname, value) GLEE_GuardedGLCall(glPatchParameteri, void, glPatchParameteri_RealName, 0, pname, value)
#define glPauseTransformFeedback() GLEE_GuardedGLCall(glPauseTransformFeedback, void, glPauseTransformFeedback_RealName, 0)
#define glPixelMapfv(map, mapsize, values) GLEE_GuardedGLCall(glPixelMapfv, void, glPixelMapfv_RealName, 0, map, mapsize, values)
#define glPixelMapuiv(map, mapsize, values) GLEE_GuardedGLCall(glPixelMapuiv, void, glPixelMapuiv_RealName, 0, map, mapsize, values)
#define glPixelMapusv(map, mapsize, values) GLEE_GuardedGLCall(glPixelMapusv, void, glPixelMapusv_RealName, 0, map, mapsize, values)
#define glPixelStoref(pname, param) GLEE_GuardedGLCall(glPixelStoref, void, glPixelStoref_RealName, 0, pname, param)
#define glPixelStorei(pname, param) GLEE_GuardedGLCall(glPixelStorei, void, glPixelStorei_RealName, 0, pname, param)
#define glPixelTransferf(pname, param) GLEE_GuardedGLCall(glPixelTransferf, void, glPixelTransferf_RealName, 0, pname, param)
#define glPixelTransferi(pname, param) GLEE_GuardedGLCall(glPixelTransferi, void, glPixelTransferi_RealName, 0, pname, param)
#define glPixelZoom(xfactor, yfactor) GLEE_GuardedGLCall(glPixelZoom, void, glPixelZoom_RealName, 0, xfactor, yfactor)
#define glPointParameterf(pname, param) GLEE_GuardedGLCall(glPointParameterf, void, glPointParameterf_RealName, 0, pname, param)
#define glPointParameterfv(pname, params) GLEE_GuardedGLCall(glPointParameterfv, void, glPointParameterfv_RealName, 0, pname, params)
#define glPointParameteri(pname, param) GLEE_GuardedGLCall(glPointParameteri, void, glPointParameteri_RealName, 0, pname, param)
#define glPointParameteriv(pname, params) GLEE_GuardedGLCall(glPointParameteriv, void, glPointParameteriv_RealName, 0, pname, params)
#define glPointSize(size) GLEE_GuardedGLCall(glPointSize, void, glPointSize_RealName, 0, size)
#define glPolygonMode(face, mode) GLEE_GuardedGLCall(glPolygonMode, void, glPolygonMode_RealName, 0, face, mode)
#define glPolygonOffset(factor, units) GLEE_GuardedGLCall(glPolygonOffset, void, glPolygonOffset_RealName, 0, factor, units)
#define glPolygonStipple(mask) GLEE_GuardedGLCall(glPolygonStipple, void, glPolygonStipple_RealName, 0, mask)
#define glPopAttrib() GLEE_GuardedGLCall(glPopAttrib, void, glPopAttrib_RealName, 0)
#define glPopClientAttrib() GLEE_GuardedGLCall(glPopClientAttrib, void, glPopClientAttrib_RealName, 0)
#define glPopMatrix() GLEE_GuardedGLCall(glPopMatrix, void, glPopMatrix_RealName, 0)
#define glPopName() GLEE_GuardedGLCall(glPopName, void, glPopName_RealName, 0)
#define glPrimitiveRestartIndex(index) GLEE_GuardedGLCall(glPrimitiveRestartIndex, void, glPrimitiveRestartIndex_RealName, 0, index)
#define glPrioritizeTextures(n, textures, priorities) GLEE_GuardedGLCall(glPrioritizeTextures, void, glPrioritizeTextures_RealName, 0, n, textures, priorities)
#define glProgramBinary(program, binaryFormat, binary, length) GLEE_GuardedGLCall(glProgramBinary, void, glProgramBinary_RealName, 0, program, binaryFormat, binary, length)
#define glProgramParameteri(program, pname, value) GLEE_GuardedGLCall(glProgramParameteri, void, glProgramParameteri_RealName, 0, program, pname, value)
#define glProgramUniform1d(program, location, v0) GLEE_GuardedGLCall(glProgramUniform1d, void, glProgramUniform1d_RealName, 0, program, location, v0)
#define glProgramUniform1dv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform1dv, void, glProgramUniform1dv_RealName, 0, program, location, count, value)
#define glProgramUniform1f(program, location, v0) GLEE_GuardedGLCall(glProgramUniform1f, void, glProgramUniform1f_RealName, 0, program, location, v0)
#define glProgramUniform1fv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform1fv, void, glProgramUniform1fv_RealName, 0, program, location, count, value)
#define glProgramUniform1i(program, location, v0) GLEE_GuardedGLCall(glProgramUniform1i, void, glProgramUniform1i_RealName, 0, program, location, v0)
#define glProgramUniform1iv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform1iv, void, glProgramUniform1iv_RealName, 0, program, location, count, value)
#define glProgramUniform1ui(program, location, v0) GLEE_GuardedGLCall(glProgramUniform1ui, void, glProgramUniform1ui_RealName, 0, program, location, v0)
#define glProgramUniform1uiv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform1uiv, void, glProgramUniform1uiv_RealName, 0, program, location, count, value)
#define glProgramUniform2d(program, location, v0, v1) GLEE_GuardedGLCall(glProgramUniform2d, void, glProgramUniform2d_RealName, 0, program, location, v0, v1)
#define glProgramUniform2dv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform2dv, void, glProgramUniform2dv_RealName, 0, program, location, count, value)
#define glProgramUniform2f(program, location, v0, v1) GLEE_GuardedGLCall(glProgramUniform2f, void, glProgramUniform2f_RealName, 0, program, location, v0, v1)
#define glProgramUniform2fv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform2fv, void, glProgramUniform2fv_RealName, 0, program, location, count, value)
#define glProgramUniform2i(program, location, v0, v1) GLEE_GuardedGLCall(glProgramUniform2i, void, glProgramUniform2i_RealName, 0, program, location, v0, v1)
#define glProgramUniform2iv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform2iv, void, glProgramUniform2iv_RealName, 0, program, location, count, value)
#define glProgramUniform2ui(program, location, v0, v1) GLEE_GuardedGLCall(glProgramUniform2ui, void, glProgramUniform2ui_RealName, 0, program, location, v0, v1)
#define glProgramUniform2uiv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform2uiv, void, glProgramUniform2uiv_RealName, 0, program, location, count, value)
#define glProgramUniform3d(program, location, v0, v1, v2) GLEE_GuardedGLCall(glProgramUniform3d, void, glProgramUniform3d_RealName, 0, program, location, v0, v1, v2)
#define glProgramUniform3dv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform3dv, void, glProgramUniform3dv_RealName, 0, program, location, count, value)
#define glProgramUniform3f(program, location, v0, v1, v2) GLEE_GuardedGLCall(glProgramUniform3f, void, glProgramUniform3f_RealName, 0, program, location, v0, v1, v2)
#define glProgramUniform3fv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform3fv, void, glProgramUniform3fv_RealName, 0, program, location, count, value)
#define glProgramUniform3i(program, location, v0, v1, v2) GLEE_GuardedGLCall(glProgramUniform3i, void, glProgramUniform3i_RealName, 0, program, location, v0, v1, v2)
#define glProgramUniform3iv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform3iv, void, glProgramUniform3iv_RealName, 0, program, location, count, value)
#define glProgramUniform3ui(program, location, v0, v1, v2) GLEE_GuardedGLCall(glProgramUniform3ui, void, glProgramUniform3ui_RealName, 0, program, location, v0, v1, v2)
#define glProgramUniform3uiv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform3uiv, void, glProgramUniform3uiv_RealName, 0, program, location, count, value)
#define glProgramUniform4d(program, location, v0, v1, v2, v3) GLEE_GuardedGLCall(glProgramUniform4d, void, glProgramUniform4d_RealName, 0, program, location, v0, v1, v2, v3)
#define glProgramUniform4dv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform4dv, void, glProgramUniform4dv_RealName, 0, program, location, count, value)
#define glProgramUniform4f(program, location, v0, v1, v2, v3) GLEE_GuardedGLCall(glProgramUniform4f, void, glProgramUniform4f_RealName, 0, program, location, v0, v1, v2, v3)
#define glProgramUniform4fv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform4fv, void, glProgramUniform4fv_RealName, 0, program, location, count, value)
#define glProgramUniform4i(program, location, v0, v1, v2, v3) GLEE_GuardedGLCall(glProgramUniform4i, void, glProgramUniform4i_RealName, 0, program, location, v0, v1, v2, v3)
#define glProgramUniform4iv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform4iv, void, glProgramUniform4iv_RealName, 0, program, location, count, value)
#define glProgramUniform4ui(program, location, v0, v1, v2, v3) GLEE_GuardedGLCall(glProgramUniform4ui, void, glProgramUniform4ui_RealName, 0, program, location, v0, v1, v2, v3)
#define glProgramUniform4uiv(program, location, count, value) GLEE_GuardedGLCall(glProgramUniform4uiv, void, glProgramUniform4uiv_RealName, 0, program, location, count, value)
#define glProgramUniformMatrix2dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix2dv, void, glProgramUniformMatrix2dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix2fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix2fv, void, glProgramUniformMatrix2fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix2x3dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix2x3dv, void, glProgramUniformMatrix2x3dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix2x3fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix2x3fv, void, glProgramUniformMatrix2x3fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix2x4dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix2x4dv, void, glProgramUniformMatrix2x4dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix2x4fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix2x4fv, void, glProgramUniformMatrix2x4fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix3dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix3dv, void, glProgramUniformMatrix3dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix3fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix3fv, void, glProgramUniformMatrix3fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix3x2dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix3x2dv, void, glProgramUniformMatrix3x2dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix3x2fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix3x2fv, void, glProgramUniformMatrix3x2fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix3x4dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix3x4dv, void, glProgramUniformMatrix3x4dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix3x4fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix3x4fv, void, glProgramUniformMatrix3x4fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix4dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix4dv, void, glProgramUniformMatrix4dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix4fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix4fv, void, glProgramUniformMatrix4fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix4x2dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix4x2dv, void, glProgramUniformMatrix4x2dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix4x2fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix4x2fv, void, glProgramUniformMatrix4x2fv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix4x3dv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix4x3dv, void, glProgramUniformMatrix4x3dv_RealName, 0, program, location, count, transpose, value)
#define glProgramUniformMatrix4x3fv(program, location, count, transpose, value) GLEE_GuardedGLCall(glProgramUniformMatrix4x3fv, void, glProgramUniformMatrix4x3fv_RealName, 0, program, location, count, transpose, value)
#define glProvokingVertex(mode) GLEE_GuardedGLCall(glProvokingVertex, void, glProvokingVertex_RealName, 0, mode)
#define glPushAttrib(mask) GLEE_GuardedGLCall(glPushAttrib, void, glPushAttrib_RealName, 0, mask)
#define glPushClientAttrib(mask) GLEE_GuardedGLCall(glPushClientAttrib, void, glPushClientAttrib_RealName, 0, mask)
#define glPushMatrix() GLEE_GuardedGLCall(glPushMatrix, void, glPushMatrix_RealName, 0)
#define glPushName(name) GLEE_GuardedGLCall(glPushName, void, glPushName_RealName, 0, name)
#define glQueryCounter(id, target) GLEE_GuardedGLCall(glQueryCounter, void, glQueryCounter_RealName, 0, id, target)
#define glRasterPos2d(x, y) GLEE_GuardedGLCall(glRasterPos2d, void, glRasterPos2d_RealName, 0, x, y)
#define glRasterPos2dv(v) GLEE_GuardedGLCall(glRasterPos2dv, void, glRasterPos2dv_RealName, 0, v)
#define glRasterPos2f(x, y) GLEE_GuardedGLCall(glRasterPos2f, void, glRasterPos2f_RealName, 0, x, y)
#define glRasterPos2fv(v) GLEE_GuardedGLCall(glRasterPos2fv, void, glRasterPos2fv_RealName, 0, v)
#define glRasterPos2i(x, y) GLEE_GuardedGLCall(glRasterPos2i, void, glRasterPos2i_RealName, 0, x, y)
#define glRasterPos2iv(v) GLEE_GuardedGLCall(glRasterPos2iv, void, glRasterPos2iv_RealName, 0, v)
#define glRasterPos2s(x, y) GLEE_GuardedGLCall(glRasterPos2s, void, glRasterPos2s_RealName, 0, x, y)
#define glRasterPos2sv(v) GLEE_GuardedGLCall(glRasterPos2sv, void, glRasterPos2sv_RealName, 0, v)
#define glRasterPos3d(x, y, z) GLEE_GuardedGLCall(glRasterPos3d, void, glRasterPos3d_RealName, 0, x, y, z)
#define glRasterPos3dv(v) GLEE_GuardedGLCall(glRasterPos3dv, void, glRasterPos3dv_RealName, 0, v)
#define glRasterPos3f(x, y, z) GLEE_GuardedGLCall(glRasterPos3f, void, glRasterPos3f_RealName, 0, x, y, z)
#define glRasterPos3fv(v) GLEE_GuardedGLCall(glRasterPos3fv, void, glRasterPos3fv_RealName, 0, v)
#define glRasterPos3i(x, y, z) GLEE_GuardedGLCall(glRasterPos3i, void, glRasterPos3i_RealName, 0, x, y, z)
#define glRasterPos3iv(v) GLEE_GuardedGLCall(glRasterPos3iv, void, glRasterPos3iv_RealName, 0, v)
#define glRasterPos3s(x, y, z) GLEE_GuardedGLCall(glRasterPos3s, void, glRasterPos3s_RealName, 0, x, y, z)
#define glRasterPos3sv(v) GLEE_GuardedGLCall(glRasterPos3sv, void, glRasterPos3sv_RealName, 0, v)
#define glRasterPos4d(x, y, z, w) GLEE_GuardedGLCall(glRasterPos4d, void, glRasterPos4d_RealName, 0, x, y, z, w)
#define glRasterPos4dv(v) GLEE_GuardedGLCall(glRasterPos4dv, void, glRasterPos4dv_RealName, 0, v)
#define glRasterPos4f(x, y, z, w) GLEE_GuardedGLCall(glRasterPos4f, void, glRasterPos4f_RealName, 0, x, y, z, w)
#define glRasterPos4fv(v) GLEE_GuardedGLCall(glRasterPos4fv, void, glRasterPos4fv_RealName, 0, v)
#define glRasterPos4i(x, y, z, w) GLEE_GuardedGLCall(glRasterPos4i, void, glRasterPos4i_RealName, 0, x, y, z, w)
#define glRasterPos4iv(v) GLEE_GuardedGLCall(glRasterPos4iv, void, glRasterPos4iv_RealName, 0, v)
#define glRasterPos4s(x, y, z, w) GLEE_GuardedGLCall(glRasterPos4s, void, glRasterPos4s_RealName, 0, x, y, z, w)
#define glRasterPos4sv(v) GLEE_GuardedGLCall(glRasterPos4sv, void, glRasterPos4sv_RealName, 0, v)
#define glReadBuffer(mode) GLEE_GuardedGLCall(glReadBuffer, void, glReadBuffer_RealName, 0, mode)
#define glReadPixels(x, y, width, height, format, type, pixels) GLEE_GuardedGLCall(glReadPixels, void, glReadPixels_RealName, 0, x, y, width, height, format, type, pixels)
#define glRectd(x1, y1, x2, y2) GLEE_GuardedGLCall(glRectd, void, glRectd_RealName, 0, x1, y1, x2, y2)
#define glRectdv(v1, v2) GLEE_GuardedGLCall(glRectdv, void, glRectdv_RealName, 0, v1, v2)
#define glRectf(x1, y1, x2, y2) GLEE_GuardedGLCall(glRectf, void, glRectf_RealName, 0, x1, y1, x2, y2)
#define glRectfv(v1, v2) GLEE_GuardedGLCall(glRectfv, void, glRectfv_RealName, 0, v1, v2)
#define glRecti(x1, y1, x2, y2) GLEE_GuardedGLCall(glRecti, void, glRecti_RealName, 0, x1, y1, x2, y2)
#define glRectiv(v1, v2) GLEE_GuardedGLCall(glRectiv, void, glRectiv_RealName, 0, v1, v2)
#define glRects(x1, y1, x2, y2) GLEE_GuardedGLCall(glRects, void, glRects_RealName, 0, x1, y1, x2, y2)
#define glRectsv(v1, v2) GLEE_GuardedGLCall(glRectsv, void, glRectsv_RealName, 0, v1, v2)
#define glReleaseShaderCompiler() GLEE_GuardedGLCall(glReleaseShaderCompiler, void, glReleaseShaderCompiler_RealName, 0)
#define glRenderMode(mode) GLEE_GuardedGLCallWithReturn(glRenderMode, GLint, glRenderMode_RealName, 0, mode)
#define glRenderbufferStorage(target, internalformat, width, height) GLEE_GuardedGLCall(glRenderbufferStorage, void, glRenderbufferStorage_RealName, 0, target, internalformat, width, height)
#define glRenderbufferStorageMultisample(target, samples, internalformat, width, height) GLEE_GuardedGLCall(glRenderbufferStorageMultisample, void, glRenderbufferStorageMultisample_RealName, 0, target, samples, internalformat, width, height)
#define glResumeTransformFeedback() GLEE_GuardedGLCall(glResumeTransformFeedback, void, glResumeTransformFeedback_RealName, 0)
#define glRotated(angle, x, y, z) GLEE_GuardedGLCall(glRotated, void, glRotated_RealName, 0, angle, x, y, z)
#define glRotatef(angle, x, y, z) GLEE_GuardedGLCall(glRotatef, void, glRotatef_RealName, 0, angle, x, y, z)
#define glSampleCoverage(value, invert) GLEE_GuardedGLCall(glSampleCoverage, void, glSampleCoverage_RealName, 0, value, invert)
#define glSampleMaski(index, mask) GLEE_GuardedGLCall(glSampleMaski, void, glSampleMaski_RealName, 0, index, mask)
#define glSamplerParameterIiv(sampler, pname, param) GLEE_GuardedGLCall(glSamplerParameterIiv, void, glSamplerParameterIiv_RealName, 0, sampler, pname, param)
#define glSamplerParameterIuiv(sampler, pname, param) GLEE_GuardedGLCall(glSamplerParameterIuiv, void, glSamplerParameterIuiv_RealName, 0, sampler, pname, param)
#define glSamplerParameterf(sampler, pname, param) GLEE_GuardedGLCall(glSamplerParameterf, void, glSamplerParameterf_RealName, 0, sampler, pname, param)
#define glSamplerParameterfv(sampler, pname, param) GLEE_GuardedGLCall(glSamplerParameterfv, void, glSamplerParameterfv_RealName, 0, sampler, pname, param)
#define glSamplerParameteri(sampler, pname, param) GLEE_GuardedGLCall(glSamplerParameteri, void, glSamplerParameteri_RealName, 0, sampler, pname, param)
#define glSamplerParameteriv(sampler, pname, param) GLEE_GuardedGLCall(glSamplerParameteriv, void, glSamplerParameteriv_RealName, 0, sampler, pname, param)
#define glScaled(x, y, z) GLEE_GuardedGLCall(glScaled, void, glScaled_RealName, 0, x, y, z)
#define glScalef(x, y, z) GLEE_GuardedGLCall(glScalef, void, glScalef_RealName, 0, x, y, z)
#define glScissor(x, y, width, height) GLEE_GuardedGLCall(glScissor, void, glScissor_RealName, 0, x, y, width, height)
#define glScissorArrayv(first, count, v) GLEE_GuardedGLCall(glScissorArrayv, void, glScissorArrayv_RealName, 0, first, count, v)
#define glScissorIndexed(index, left, bottom, width, height) GLEE_GuardedGLCall(glScissorIndexed, void, glScissorIndexed_RealName, 0, index, left, bottom, width, height)
#define glScissorIndexedv(index, v) GLEE_GuardedGLCall(glScissorIndexedv, void, glScissorIndexedv_RealName, 0, index, v)
#define glSelectBuffer(size, buffer) GLEE_GuardedGLCall(glSelectBuffer, void, glSelectBuffer_RealName, 0, size, buffer)
#define glShadeModel(mode) GLEE_GuardedGLCall(glShadeModel, void, glShadeModel_RealName, 0, mode)
#define glShaderBinary(count, shaders, binaryformat, binary, length) GLEE_GuardedGLCall(glShaderBinary, void, glShaderBinary_RealName, 0, count, shaders, binaryformat, binary, length)
#define glShaderSource(shader, count, string, length) GLEE_GuardedGLCall(glShaderSource, void, glShaderSource_RealName, 0, shader, count, string, length)
#define glStencilFunc(func, ref, mask) GLEE_GuardedGLCall(glStencilFunc, void, glStencilFunc_RealName, 0, func, ref, mask)
#define glStencilFuncSeparate(face, func, ref, mask) GLEE_GuardedGLCall(glStencilFuncSeparate, void, glStencilFuncSeparate_RealName, 0, face, func, ref, mask)
#define glStencilMask(mask) GLEE_GuardedGLCall(glStencilMask, void, glStencilMask_RealName, 0, mask)
#define glStencilMaskSeparate(face, mask) GLEE_GuardedGLCall(glStencilMaskSeparate, void, glStencilMaskSeparate_RealName, 0, face, mask)
#define glStencilOp(fail, zfail, zpass) GLEE_GuardedGLCall(glStencilOp, void, glStencilOp_RealName, 0, fail, zfail, zpass)
#define glStencilOpSeparate(face, sfail, dpfail, dppass) GLEE_GuardedGLCall(glStencilOpSeparate, void, glStencilOpSeparate_RealName, 0, face, sfail, dpfail, dppass)
#define glTexBuffer(target, internalformat, buffer) GLEE_GuardedGLCall(glTexBuffer, void, glTexBuffer_RealName, 0, target, internalformat, buffer)
#define glTexCoord1d(s) GLEE_GuardedGLCall(glTexCoord1d, void, glTexCoord1d_RealName, 0, s)
#define glTexCoord1dv(v) GLEE_GuardedGLCall(glTexCoord1dv, void, glTexCoord1dv_RealName, 0, v)
#define glTexCoord1f(s) GLEE_GuardedGLCall(glTexCoord1f, void, glTexCoord1f_RealName, 0, s)
#define glTexCoord1fv(v) GLEE_GuardedGLCall(glTexCoord1fv, void, glTexCoord1fv_RealName, 0, v)
#define glTexCoord1i(s) GLEE_GuardedGLCall(glTexCoord1i, void, glTexCoord1i_RealName, 0, s)
#define glTexCoord1iv(v) GLEE_GuardedGLCall(glTexCoord1iv, void, glTexCoord1iv_RealName, 0, v)
#define glTexCoord1s(s) GLEE_GuardedGLCall(glTexCoord1s, void, glTexCoord1s_RealName, 0, s)
#define glTexCoord1sv(v) GLEE_GuardedGLCall(glTexCoord1sv, void, glTexCoord1sv_RealName, 0, v)
#define glTexCoord2d(s, t) GLEE_GuardedGLCall(glTexCoord2d, void, glTexCoord2d_RealName, 0, s, t)
#define glTexCoord2dv(v) GLEE_GuardedGLCall(glTexCoord2dv, void, glTexCoord2dv_RealName, 0, v)
#define glTexCoord2f(s, t) GLEE_GuardedGLCall(glTexCoord2f, void, glTexCoord2f_RealName, 0, s, t)
#define glTexCoord2fv(v) GLEE_GuardedGLCall(glTexCoord2fv, void, glTexCoord2fv_RealName, 0, v)
#define glTexCoord2i(s, t) GLEE_GuardedGLCall(glTexCoord2i, void, glTexCoord2i_RealName, 0, s, t)
#define glTexCoord2iv(v) GLEE_GuardedGLCall(glTexCoord2iv, void, glTexCoord2iv_RealName, 0, v)
#define glTexCoord2s(s, t) GLEE_GuardedGLCall(glTexCoord2s, void, glTexCoord2s_RealName, 0, s, t)
#define glTexCoord2sv(v) GLEE_GuardedGLCall(glTexCoord2sv, void, glTexCoord2sv_RealName, 0, v)
#define glTexCoord3d(s, t, r) GLEE_GuardedGLCall(glTexCoord3d, void, glTexCoord3d_RealName, 0, s, t, r)
#define glTexCoord3dv(v) GLEE_GuardedGLCall(glTexCoord3dv, void, glTexCoord3dv_RealName, 0, v)
#define glTexCoord3f(s, t, r) GLEE_GuardedGLCall(glTexCoord3f, void, glTexCoord3f_RealName, 0, s, t, r)
#define glTexCoord3fv(v) GLEE_GuardedGLCall(glTexCoord3fv, void, glTexCoord3fv_RealName, 0, v)
#define glTexCoord3i(s, t, r) GLEE_GuardedGLCall(glTexCoord3i, void, glTexCoord3i_RealName, 0, s, t, r)
#define glTexCoord3iv(v) GLEE_GuardedGLCall(glTexCoord3iv, void, glTexCoord3iv_RealName, 0, v)
#define glTexCoord3s(s, t, r) GLEE_GuardedGLCall(glTexCoord3s, void, glTexCoord3s_RealName, 0, s, t, r)
#define glTexCoord3sv(v) GLEE_GuardedGLCall(glTexCoord3sv, void, glTexCoord3sv_RealName, 0, v)
#define glTexCoord4d(s, t, r, q) GLEE_GuardedGLCall(glTexCoord4d, void, glTexCoord4d_RealName, 0, s, t, r, q)
#define glTexCoord4dv(v) GLEE_GuardedGLCall(glTexCoord4dv, void, glTexCoord4dv_RealName, 0, v)
#define glTexCoord4f(s, t, r, q) GLEE_GuardedGLCall(glTexCoord4f, void, glTexCoord4f_RealName, 0, s, t, r, q)
#define glTexCoord4fv(v) GLEE_GuardedGLCall(glTexCoord4fv, void, glTexCoord4fv_RealName, 0, v)
#define glTexCoord4i(s, t, r, q) GLEE_GuardedGLCall(glTexCoord4i, void, glTexCoord4i_RealName, 0, s, t, r, q)
#define glTexCoord4iv(v) GLEE_GuardedGLCall(glTexCoord4iv, void, glTexCoord4iv_RealName, 0, v)
#define glTexCoord4s(s, t, r, q) GLEE_GuardedGLCall(glTexCoord4s, void, glTexCoord4s_RealName, 0, s, t, r, q)
#define glTexCoord4sv(v) GLEE_GuardedGLCall(glTexCoord4sv, void, glTexCoord4sv_RealName, 0, v)
#define glTexCoordPointer(size, type, stride, pointer) GLEE_GuardedGLCall(glTexCoordPointer, void, glTexCoordPointer_RealName, 0, size, type, stride, pointer)
#define glTexEnvf(target, pname, param) GLEE_GuardedGLCall(glTexEnvf, void, glTexEnvf_RealName, 0, target, pname, param)
#define glTexEnvfv(target, pname, params) GLEE_GuardedGLCall(glTexEnvfv, void, glTexEnvfv_RealName, 0, target, pname, params)
#define glTexEnvi(target, pname, param) GLEE_GuardedGLCall(glTexEnvi, void, glTexEnvi_RealName, 0, target, pname, param)
#define glTexEnviv(target, pname, params) GLEE_GuardedGLCall(glTexEnviv, void, glTexEnviv_RealName, 0, target, pname, params)
#define glTexGend(coord, pname, param) GLEE_GuardedGLCall(glTexGend, void, glTexGend_RealName, 0, coord, pname, param)
#define glTexGendv(coord, pname, params) GLEE_GuardedGLCall(glTexGendv, void, glTexGendv_RealName, 0, coord, pname, params)
#define glTexGenf(coord, pname, param) GLEE_GuardedGLCall(glTexGenf, void, glTexGenf_RealName, 0, coord, pname, param)
#define glTexGenfv(coord, pname, params) GLEE_GuardedGLCall(glTexGenfv, void, glTexGenfv_RealName, 0, coord, pname, params)
#define glTexGeni(coord, pname, param) GLEE_GuardedGLCall(glTexGeni, void, glTexGeni_RealName, 0, coord, pname, param)
#define glTexGeniv(coord, pname, params) GLEE_GuardedGLCall(glTexGeniv, void, glTexGeniv_RealName, 0, coord, pname, params)
#define glTexImage1D(target, level, internalformat, width, border, format, type, pixels) GLEE_GuardedGLCall(glTexImage1D, void, glTexImage1D_RealName, 0, target, level, internalformat, width, border, format, type, pixels)
#define glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels) GLEE_GuardedGLCall(glTexImage2D, void, glTexImage2D_RealName, 0, target, level, internalformat, width, height, border, format, type, pixels)
#define glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations) GLEE_GuardedGLCall(glTexImage2DMultisample, void, glTexImage2DMultisample_RealName, 0, target, samples, internalformat, width, height, fixedsamplelocations)
#define glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels) GLEE_GuardedGLCall(glTexImage3D, void, glTexImage3D_RealName, 0, target, level, internalformat, width, height, depth, border, format, type, pixels)
#define glTexImage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations) GLEE_GuardedGLCall(glTexImage3DMultisample, void, glTexImage3DMultisample_RealName, 0, target, samples, internalformat, width, height, depth, fixedsamplelocations)
#define glTexParameterIiv(target, pname, params) GLEE_GuardedGLCall(glTexParameterIiv, void, glTexParameterIiv_RealName, 0, target, pname, params)
#define glTexParameterIuiv(target, pname, params) GLEE_GuardedGLCall(glTexParameterIuiv, void, glTexParameterIuiv_RealName, 0, target, pname, params)
#define glTexParameterf(target, pname, param) GLEE_GuardedGLCall(glTexParameterf, void, glTexParameterf_RealName, 0, target, pname, param)
#define glTexParameterfv(target, pname, params) GLEE_GuardedGLCall(glTexParameterfv, void, glTexParameterfv_RealName, 0, target, pname, params)
#define glTexParameteri(target, pname, param) GLEE_GuardedGLCall(glTexParameteri, void, glTexParameteri_RealName, 0, target, pname, param)
#define glTexParameteriv(target, pname, params) GLEE_GuardedGLCall(glTexParameteriv, void, glTexParameteriv_RealName, 0, target, pname, params)
#define glTexSubImage1D(target, level, xoffset, width, format, type, pixels) GLEE_GuardedGLCall(glTexSubImage1D, void, glTexSubImage1D_RealName, 0, target, level, xoffset, width, format, type, pixels)
#define glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels) GLEE_GuardedGLCall(glTexSubImage2D, void, glTexSubImage2D_RealName, 0, target, level, xoffset, yoffset, width, height, format, type, pixels)
#define glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels) GLEE_GuardedGLCall(glTexSubImage3D, void, glTexSubImage3D_RealName, 0, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels)
#define glTransformFeedbackVaryings(program, count, varyings, bufferMode) GLEE_GuardedGLCall(glTransformFeedbackVaryings, void, glTransformFeedbackVaryings_RealName, 0, program, count, varyings, bufferMode)
#define glTranslated(x, y, z) GLEE_GuardedGLCall(glTranslated, void, glTranslated_RealName, 0, x, y, z)
#define glTranslatef(x, y, z) GLEE_GuardedGLCall(glTranslatef, void, glTranslatef_RealName, 0, x, y, z)
#define glUniform1d(location, x) GLEE_GuardedGLCall(glUniform1d, void, glUniform1d_RealName, location, location, x)
#define glUniform1dv(location, count, value) GLEE_GuardedGLCall(glUniform1dv, void, glUniform1dv_RealName, location, location, count, value)
#define glUniform1f(location, v0) GLEE_GuardedGLCall(glUniform1f, void, glUniform1f_RealName, location, location, v0)
#define glUniform1fv(location, count, value) GLEE_GuardedGLCall(glUniform1fv, void, glUniform1fv_RealName, location, location, count, value)
#define glUniform1i(location, v0) GLEE_GuardedGLCall(glUniform1i, void, glUniform1i_RealName, location, location, v0)
#define glUniform1iv(location, count, value) GLEE_GuardedGLCall(glUniform1iv, void, glUniform1iv_RealName, location, location, count, value)
#define glUniform1ui(location, v0) GLEE_GuardedGLCall(glUniform1ui, void, glUniform1ui_RealName, location, location, v0)
#define glUniform1uiv(location, count, value) GLEE_GuardedGLCall(glUniform1uiv, void, glUniform1uiv_RealName, location, location, count, value)
#define glUniform2d(location, x, y) GLEE_GuardedGLCall(glUniform2d, void, glUniform2d_RealName, location, location, x, y)
#define glUniform2dv(location, count, value) GLEE_GuardedGLCall(glUniform2dv, void, glUniform2dv_RealName, location, location, count, value)
#define glUniform2f(location, v0, v1) GLEE_GuardedGLCall(glUniform2f, void, glUniform2f_RealName, location, location, v0, v1)
#define glUniform2fv(location, count, value) GLEE_GuardedGLCall(glUniform2fv, void, glUniform2fv_RealName, location, location, count, value)
#define glUniform2i(location, v0, v1) GLEE_GuardedGLCall(glUniform2i, void, glUniform2i_RealName, location, location, v0, v1)
#define glUniform2iv(location, count, value) GLEE_GuardedGLCall(glUniform2iv, void, glUniform2iv_RealName, location, location, count, value)
#define glUniform2ui(location, v0, v1) GLEE_GuardedGLCall(glUniform2ui, void, glUniform2ui_RealName, location, location, v0, v1)
#define glUniform2uiv(location, count, value) GLEE_GuardedGLCall(glUniform2uiv, void, glUniform2uiv_RealName, location, location, count, value)
#define glUniform3d(location, x, y, z) GLEE_GuardedGLCall(glUniform3d, void, glUniform3d_RealName, location, location, x, y, z)
#define glUniform3dv(location, count, value) GLEE_GuardedGLCall(glUniform3dv, void, glUniform3dv_RealName, location, location, count, value)
#define glUniform3f(location, v0, v1, v2) GLEE_GuardedGLCall(glUniform3f, void, glUniform3f_RealName, location, location, v0, v1, v2)
#define glUniform3fv(location, count, value) GLEE_GuardedGLCall(glUniform3fv, void, glUniform3fv_RealName, location, location, count, value)
#define glUniform3i(location, v0, v1, v2) GLEE_GuardedGLCall(glUniform3i, void, glUniform3i_RealName, location, location, v0, v1, v2)
#define glUniform3iv(location, count, value) GLEE_GuardedGLCall(glUniform3iv, void, glUniform3iv_RealName, location, location, count, value)
#define glUniform3ui(location, v0, v1, v2) GLEE_GuardedGLCall(glUniform3ui, void, glUniform3ui_RealName, location, location, v0, v1, v2)
#define glUniform3uiv(location, count, value) GLEE_GuardedGLCall(glUniform3uiv, void, glUniform3uiv_RealName, location, location, count, value)
#define glUniform4d(location, x, y, z, w) GLEE_GuardedGLCall(glUniform4d, void, glUniform4d_RealName, location, location, x, y, z, w)
#define glUniform4dv(location, count, value) GLEE_GuardedGLCall(glUniform4dv, void, glUniform4dv_RealName, location, location, count, value)
#define glUniform4f(location, v0, v1, v2, v3) GLEE_GuardedGLCall(glUniform4f, void, glUniform4f_RealName, location, location, v0, v1, v2, v3)
#define glUniform4fv(location, count, value) GLEE_GuardedGLCall(glUniform4fv, void, glUniform4fv_RealName, location, location, count, value)
#define glUniform4i(location, v0, v1, v2, v3) GLEE_GuardedGLCall(glUniform4i, void, glUniform4i_RealName, location, location, v0, v1, v2, v3)
#define glUniform4iv(location, count, value) GLEE_GuardedGLCall(glUniform4iv, void, glUniform4iv_RealName, location, location, count, value)
#define glUniform4ui(location, v0, v1, v2, v3) GLEE_GuardedGLCall(glUniform4ui, void, glUniform4ui_RealName, location, location, v0, v1, v2, v3)
#define glUniform4uiv(location, count, value) GLEE_GuardedGLCall(glUniform4uiv, void, glUniform4uiv_RealName, location, location, count, value)
#define glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding) GLEE_GuardedGLCall(glUniformBlockBinding, void, glUniformBlockBinding_RealName, program, program, uniformBlockIndex, uniformBlockBinding)
#define glUniformMatrix2dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix2dv, void, glUniformMatrix2dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix2fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix2fv, void, glUniformMatrix2fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix2x3dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix2x3dv, void, glUniformMatrix2x3dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix2x3fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix2x3fv, void, glUniformMatrix2x3fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix2x4dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix2x4dv, void, glUniformMatrix2x4dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix2x4fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix2x4fv, void, glUniformMatrix2x4fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix3dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix3dv, void, glUniformMatrix3dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix3fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix3fv, void, glUniformMatrix3fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix3x2dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix3x2dv, void, glUniformMatrix3x2dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix3x2fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix3x2fv, void, glUniformMatrix3x2fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix3x4dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix3x4dv, void, glUniformMatrix3x4dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix3x4fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix3x4fv, void, glUniformMatrix3x4fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix4dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix4dv, void, glUniformMatrix4dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix4fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix4fv, void, glUniformMatrix4fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix4x2dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix4x2dv, void, glUniformMatrix4x2dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix4x2fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix4x2fv, void, glUniformMatrix4x2fv_RealName, location, location, count, transpose, value)
#define glUniformMatrix4x3dv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix4x3dv, void, glUniformMatrix4x3dv_RealName, location, location, count, transpose, value)
#define glUniformMatrix4x3fv(location, count, transpose, value) GLEE_GuardedGLCall(glUniformMatrix4x3fv, void, glUniformMatrix4x3fv_RealName, location, location, count, transpose, value)
#define glUniformSubroutinesuiv(shadertype, count, indices) GLEE_GuardedGLCall(glUniformSubroutinesuiv, void, glUniformSubroutinesuiv_RealName, shadertype, shadertype, count, indices)
#define glUnmapBuffer(target) GLEE_GuardedGLCallWithReturn(glUnmapBuffer, GLboolean, glUnmapBuffer_RealName, 0, target)
#define glUseProgram(program) GLEE_GuardedGLCall(glUseProgram, void, glUseProgram_RealName, 0, program)
#define glUseProgramStages(pipeline, stages, program) GLEE_GuardedGLCall(glUseProgramStages, void, glUseProgramStages_RealName, 0, pipeline, stages, program)
#define glValidateProgram(program) GLEE_GuardedGLCall(glValidateProgram, void, glValidateProgram_RealName, 0, program)
#define glValidateProgramPipeline(pipeline) GLEE_GuardedGLCall(glValidateProgramPipeline, void, glValidateProgramPipeline_RealName, 0, pipeline)
#define glVertex2d(x, y) GLEE_GuardedGLCall(glVertex2d, void, glVertex2d_RealName, 0, x, y)
#define glVertex2dv(v) GLEE_GuardedGLCall(glVertex2dv, void, glVertex2dv_RealName, 0, v)
#define glVertex2f(x, y) GLEE_GuardedGLCall(glVertex2f, void, glVertex2f_RealName, 0, x, y)
#define glVertex2fv(v) GLEE_GuardedGLCall(glVertex2fv, void, glVertex2fv_RealName, 0, v)
#define glVertex2i(x, y) GLEE_GuardedGLCall(glVertex2i, void, glVertex2i_RealName, 0, x, y)
#define glVertex2iv(v) GLEE_GuardedGLCall(glVertex2iv, void, glVertex2iv_RealName, 0, v)
#define glVertex2s(x, y) GLEE_GuardedGLCall(glVertex2s, void, glVertex2s_RealName, 0, x, y)
#define glVertex2sv(v) GLEE_GuardedGLCall(glVertex2sv, void, glVertex2sv_RealName, 0, v)
#define glVertex3d(x, y, z) GLEE_GuardedGLCall(glVertex3d, void, glVertex3d_RealName, 0, x, y, z)
#define glVertex3dv(v) GLEE_GuardedGLCall(glVertex3dv, void, glVertex3dv_RealName, 0, v)
#define glVertex3f(x, y, z) GLEE_GuardedGLCall(glVertex3f, void, glVertex3f_RealName, 0, x, y, z)
#define glVertex3fv(v) GLEE_GuardedGLCall(glVertex3fv, void, glVertex3fv_RealName, 0, v)
#define glVertex3i(x, y, z) GLEE_GuardedGLCall(glVertex3i, void, glVertex3i_RealName, 0, x, y, z)
#define glVertex3iv(v) GLEE_GuardedGLCall(glVertex3iv, void, glVertex3iv_RealName, 0, v)
#define glVertex3s(x, y, z) GLEE_GuardedGLCall(glVertex3s, void, glVertex3s_RealName, 0, x, y, z)
#define glVertex3sv(v) GLEE_GuardedGLCall(glVertex3sv, void, glVertex3sv_RealName, 0, v)
#define glVertex4d(x, y, z, w) GLEE_GuardedGLCall(glVertex4d, void, glVertex4d_RealName, 0, x, y, z, w)
#define glVertex4dv(v) GLEE_GuardedGLCall(glVertex4dv, void, glVertex4dv_RealName, 0, v)
#define glVertex4f(x, y, z, w) GLEE_GuardedGLCall(glVertex4f, void, glVertex4f_RealName, 0, x, y, z, w)
#define glVertex4fv(v) GLEE_GuardedGLCall(glVertex4fv, void, glVertex4fv_RealName, 0, v)
#define glVertex4i(x, y, z, w) GLEE_GuardedGLCall(glVertex4i, void, glVertex4i_RealName, 0, x, y, z, w)
#define glVertex4iv(v) GLEE_GuardedGLCall(glVertex4iv, void, glVertex4iv_RealName, 0, v)
#define glVertex4s(x, y, z, w) GLEE_GuardedGLCall(glVertex4s, void, glVertex4s_RealName, 0, x, y, z, w)
#define glVertex4sv(v) GLEE_GuardedGLCall(glVertex4sv, void, glVertex4sv_RealName, 0, v)
#define glVertexAttrib1d(index, x) GLEE_GuardedGLCall(glVertexAttrib1d, void, glVertexAttrib1d_RealName, 0, index, x)
#define glVertexAttrib1dv(index, v) GLEE_GuardedGLCall(glVertexAttrib1dv, void, glVertexAttrib1dv_RealName, 0, index, v)
#define glVertexAttrib1f(index, x) GLEE_GuardedGLCall(glVertexAttrib1f, void, glVertexAttrib1f_RealName, 0, index, x)
#define glVertexAttrib1fv(index, v) GLEE_GuardedGLCall(glVertexAttrib1fv, void, glVertexAttrib1fv_RealName, 0, index, v)
#define glVertexAttrib1s(index, x) GLEE_GuardedGLCall(glVertexAttrib1s, void, glVertexAttrib1s_RealName, 0, index, x)
#define glVertexAttrib1sv(index, v) GLEE_GuardedGLCall(glVertexAttrib1sv, void, glVertexAttrib1sv_RealName, 0, index, v)
#define glVertexAttrib2d(index, x, y) GLEE_GuardedGLCall(glVertexAttrib2d, void, glVertexAttrib2d_RealName, 0, index, x, y)
#define glVertexAttrib2dv(index, v) GLEE_GuardedGLCall(glVertexAttrib2dv, void, glVertexAttrib2dv_RealName, 0, index, v)
#define glVertexAttrib2f(index, x, y) GLEE_GuardedGLCall(glVertexAttrib2f, void, glVertexAttrib2f_RealName, 0, index, x, y)
#define glVertexAttrib2fv(index, v) GLEE_GuardedGLCall(glVertexAttrib2fv, void, glVertexAttrib2fv_RealName, 0, index, v)
#define glVertexAttrib2s(index, x, y) GLEE_GuardedGLCall(glVertexAttrib2s, void, glVertexAttrib2s_RealName, 0, index, x, y)
#define glVertexAttrib2sv(index, v) GLEE_GuardedGLCall(glVertexAttrib2sv, void, glVertexAttrib2sv_RealName, 0, index, v)
#define glVertexAttrib3d(index, x, y, z) GLEE_GuardedGLCall(glVertexAttrib3d, void, glVertexAttrib3d_RealName, 0, index, x, y, z)
#define glVertexAttrib3dv(index, v) GLEE_GuardedGLCall(glVertexAttrib3dv, void, glVertexAttrib3dv_RealName, 0, index, v)
#define glVertexAttrib3f(index, x, y, z) GLEE_GuardedGLCall(glVertexAttrib3f, void, glVertexAttrib3f_RealName, 0, index, x, y, z)
#define glVertexAttrib3fv(index, v) GLEE_GuardedGLCall(glVertexAttrib3fv, void, glVertexAttrib3fv_RealName, 0, index, v)
#define glVertexAttrib3s(index, x, y, z) GLEE_GuardedGLCall(glVertexAttrib3s, void, glVertexAttrib3s_RealName, 0, index, x, y, z)
#define glVertexAttrib3sv(index, v) GLEE_GuardedGLCall(glVertexAttrib3sv, void, glVertexAttrib3sv_RealName, 0, index, v)
#define glVertexAttrib4Nbv(index, v) GLEE_GuardedGLCall(glVertexAttrib4Nbv, void, glVertexAttrib4Nbv_RealName, 0, index, v)
#define glVertexAttrib4Niv(index, v) GLEE_GuardedGLCall(glVertexAttrib4Niv, void, glVertexAttrib4Niv_RealName, 0, index, v)
#define glVertexAttrib4Nsv(index, v) GLEE_GuardedGLCall(glVertexAttrib4Nsv, void, glVertexAttrib4Nsv_RealName, 0, index, v)
#define glVertexAttrib4Nub(index, x, y, z, w) GLEE_GuardedGLCall(glVertexAttrib4Nub, void, glVertexAttrib4Nub_RealName, 0, index, x, y, z, w)
#define glVertexAttrib4Nubv(index, v) GLEE_GuardedGLCall(glVertexAttrib4Nubv, void, glVertexAttrib4Nubv_RealName, 0, index, v)
#define glVertexAttrib4Nuiv(index, v) GLEE_GuardedGLCall(glVertexAttrib4Nuiv, void, glVertexAttrib4Nuiv_RealName, 0, index, v)
#define glVertexAttrib4Nusv(index, v) GLEE_GuardedGLCall(glVertexAttrib4Nusv, void, glVertexAttrib4Nusv_RealName, 0, index, v)
#define glVertexAttrib4bv(index, v) GLEE_GuardedGLCall(glVertexAttrib4bv, void, glVertexAttrib4bv_RealName, 0, index, v)
#define glVertexAttrib4d(index, x, y, z, w) GLEE_GuardedGLCall(glVertexAttrib4d, void, glVertexAttrib4d_RealName, 0, index, x, y, z, w)
#define glVertexAttrib4dv(index, v) GLEE_GuardedGLCall(glVertexAttrib4dv, void, glVertexAttrib4dv_RealName, 0, index, v)
#define glVertexAttrib4f(index, x, y, z, w) GLEE_GuardedGLCall(glVertexAttrib4f, void, glVertexAttrib4f_RealName, 0, index, x, y, z, w)
#define glVertexAttrib4fv(index, v) GLEE_GuardedGLCall(glVertexAttrib4fv, void, glVertexAttrib4fv_RealName, 0, index, v)
#define glVertexAttrib4iv(index, v) GLEE_GuardedGLCall(glVertexAttrib4iv, void, glVertexAttrib4iv_RealName, 0, index, v)
#define glVertexAttrib4s(index, x, y, z, w) GLEE_GuardedGLCall(glVertexAttrib4s, void, glVertexAttrib4s_RealName, 0, index, x, y, z, w)
#define glVertexAttrib4sv(index, v) GLEE_GuardedGLCall(glVertexAttrib4sv, void, glVertexAttrib4sv_RealName, 0, index, v)
#define glVertexAttrib4ubv(index, v) GLEE_GuardedGLCall(glVertexAttrib4ubv, void, glVertexAttrib4ubv_RealName, 0, index, v)
#define glVertexAttrib4uiv(index, v) GLEE_GuardedGLCall(glVertexAttrib4uiv, void, glVertexAttrib4uiv_RealName, 0, index, v)
#define glVertexAttrib4usv(index, v) GLEE_GuardedGLCall(glVertexAttrib4usv, void, glVertexAttrib4usv_RealName, 0, index, v)
#define glVertexAttribDivisor(index, divisor) GLEE_GuardedGLCall(glVertexAttribDivisor, void, glVertexAttribDivisor_RealName, 0, index, divisor)
#define glVertexAttribI1i(index, x) GLEE_GuardedGLCall(glVertexAttribI1i, void, glVertexAttribI1i_RealName, 0, index, x)
#define glVertexAttribI1iv(index, v) GLEE_GuardedGLCall(glVertexAttribI1iv, void, glVertexAttribI1iv_RealName, 0, index, v)
#define glVertexAttribI1ui(index, x) GLEE_GuardedGLCall(glVertexAttribI1ui, void, glVertexAttribI1ui_RealName, 0, index, x)
#define glVertexAttribI1uiv(index, v) GLEE_GuardedGLCall(glVertexAttribI1uiv, void, glVertexAttribI1uiv_RealName, 0, index, v)
#define glVertexAttribI2i(index, x, y) GLEE_GuardedGLCall(glVertexAttribI2i, void, glVertexAttribI2i_RealName, 0, index, x, y)
#define glVertexAttribI2iv(index, v) GLEE_GuardedGLCall(glVertexAttribI2iv, void, glVertexAttribI2iv_RealName, 0, index, v)
#define glVertexAttribI2ui(index, x, y) GLEE_GuardedGLCall(glVertexAttribI2ui, void, glVertexAttribI2ui_RealName, 0, index, x, y)
#define glVertexAttribI2uiv(index, v) GLEE_GuardedGLCall(glVertexAttribI2uiv, void, glVertexAttribI2uiv_RealName, 0, index, v)
#define glVertexAttribI3i(index, x, y, z) GLEE_GuardedGLCall(glVertexAttribI3i, void, glVertexAttribI3i_RealName, 0, index, x, y, z)
#define glVertexAttribI3iv(index, v) GLEE_GuardedGLCall(glVertexAttribI3iv, void, glVertexAttribI3iv_RealName, 0, index, v)
#define glVertexAttribI3ui(index, x, y, z) GLEE_GuardedGLCall(glVertexAttribI3ui, void, glVertexAttribI3ui_RealName, 0, index, x, y, z)
#define glVertexAttribI3uiv(index, v) GLEE_GuardedGLCall(glVertexAttribI3uiv, void, glVertexAttribI3uiv_RealName, 0, index, v)
#define glVertexAttribI4bv(index, v) GLEE_GuardedGLCall(glVertexAttribI4bv, void, glVertexAttribI4bv_RealName, 0, index, v)
#define glVertexAttribI4i(index, x, y, z, w) GLEE_GuardedGLCall(glVertexAttribI4i, void, glVertexAttribI4i_RealName, 0, index, x, y, z, w)
#define glVertexAttribI4iv(index, v) GLEE_GuardedGLCall(glVertexAttribI4iv, void, glVertexAttribI4iv_RealName, 0, index, v)
#define glVertexAttribI4sv(index, v) GLEE_GuardedGLCall(glVertexAttribI4sv, void, glVertexAttribI4sv_RealName, 0, index, v)
#define glVertexAttribI4ubv(index, v) GLEE_GuardedGLCall(glVertexAttribI4ubv, void, glVertexAttribI4ubv_RealName, 0, index, v)
#define glVertexAttribI4ui(index, x, y, z, w) GLEE_GuardedGLCall(glVertexAttribI4ui, void, glVertexAttribI4ui_RealName, 0, index, x, y, z, w)
#define glVertexAttribI4uiv(index, v) GLEE_GuardedGLCall(glVertexAttribI4uiv, void, glVertexAttribI4uiv_RealName, 0, index, v)
#define glVertexAttribI4usv(index, v) GLEE_GuardedGLCall(glVertexAttribI4usv, void, glVertexAttribI4usv_RealName, 0, index, v)
#define glVertexAttribIPointer(index, size, type, stride, pointer) GLEE_GuardedGLCall(glVertexAttribIPointer, void, glVertexAttribIPointer_RealName, 0, index, size, type, stride, pointer)
#define glVertexAttribL1d(index, x) GLEE_GuardedGLCall(glVertexAttribL1d, void, glVertexAttribL1d_RealName, 0, index, x)
#define glVertexAttribL1dv(index, v) GLEE_GuardedGLCall(glVertexAttribL1dv, void, glVertexAttribL1dv_RealName, 0, index, v)
#define glVertexAttribL2d(index, x, y) GLEE_GuardedGLCall(glVertexAttribL2d, void, glVertexAttribL2d_RealName, 0, index, x, y)
#define glVertexAttribL2dv(index, v) GLEE_GuardedGLCall(glVertexAttribL2dv, void, glVertexAttribL2dv_RealName, 0, index, v)
#define glVertexAttribL3d(index, x, y, z) GLEE_GuardedGLCall(glVertexAttribL3d, void, glVertexAttribL3d_RealName, 0, index, x, y, z)
#define glVertexAttribL3dv(index, v) GLEE_GuardedGLCall(glVertexAttribL3dv, void, glVertexAttribL3dv_RealName, 0, index, v)
#define glVertexAttribL4d(index, x, y, z, w) GLEE_GuardedGLCall(glVertexAttribL4d, void, glVertexAttribL4d_RealName, 0, index, x, y, z, w)
#define glVertexAttribL4dv(index, v) GLEE_GuardedGLCall(glVertexAttribL4dv, void, glVertexAttribL4dv_RealName, 0, index, v)
#define glVertexAttribLPointer(index, size, type, stride, pointer) GLEE_GuardedGLCall(glVertexAttribLPointer, void, glVertexAttribLPointer_RealName, 0, index, size, type, stride, pointer)
#define glVertexAttribP1ui(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP1ui, void, glVertexAttribP1ui_RealName, 0, index, type, normalized, value)
#define glVertexAttribP1uiv(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP1uiv, void, glVertexAttribP1uiv_RealName, 0, index, type, normalized, value)
#define glVertexAttribP2ui(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP2ui, void, glVertexAttribP2ui_RealName, 0, index, type, normalized, value)
#define glVertexAttribP2uiv(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP2uiv, void, glVertexAttribP2uiv_RealName, 0, index, type, normalized, value)
#define glVertexAttribP3ui(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP3ui, void, glVertexAttribP3ui_RealName, 0, index, type, normalized, value)
#define glVertexAttribP3uiv(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP3uiv, void, glVertexAttribP3uiv_RealName, 0, index, type, normalized, value)
#define glVertexAttribP4ui(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP4ui, void, glVertexAttribP4ui_RealName, 0, index, type, normalized, value)
#define glVertexAttribP4uiv(index, type, normalized, value) GLEE_GuardedGLCall(glVertexAttribP4uiv, void, glVertexAttribP4uiv_RealName, 0, index, type, normalized, value)
#define glVertexAttribPointer(index, size, type, normalized, stride, pointer) GLEE_GuardedGLCall(glVertexAttribPointer, void, glVertexAttribPointer_RealName, 0, index, size, type, normalized, stride, pointer)
#define glVertexPointer(size, type, stride, pointer) GLEE_GuardedGLCall(glVertexPointer, void, glVertexPointer_RealName, 0, size, type, stride, pointer)
#define glViewport(x, y, width, height) GLEE_GuardedGLCall(glViewport, void, glViewport_RealName, 0, x, y, width, height)
#define glViewportArrayv(first, count, v) GLEE_GuardedGLCall(glViewportArrayv, void, glViewportArrayv_RealName, 0, first, count, v)
#define glViewportIndexedf(index, x, y, w, h) GLEE_GuardedGLCall(glViewportIndexedf, void, glViewportIndexedf_RealName, 0, index, x, y, w, h)
#define glViewportIndexedfv(index, v) GLEE_GuardedGLCall(glViewportIndexedfv, void, glViewportIndexedfv_RealName, 0, index, v)
#define glWaitSync(sync, flags, timeout) GLEE_GuardedGLCall(glWaitSync, void, glWaitSync_RealName, 0, sync, flags, timeout)

#endif // defined(GLEE_OVERWRITE_GL_FUNCTIONS)



#endif // defined(glee_hpp)

