
#include "utility.hpp"

#if defined(__APPLE__)
std::string getExecutableDirectory()
{
    CFStringRef sRef = CFURLCopyPath(CFBundleCopyExecutableURL(CFBundleGetMainBundle()));
    char path[PATH_MAX];
    CFStringGetCString(sRef, path, sizeof(path), kCFStringEncodingASCII);
    
    char* loc = strrchr(path, '/');
    if(loc != NULL)
        loc[0] = '\0';
    
    for(int i=0; i < 2; ++i)
    {
    loc = strrchr(path, '/');
    if(loc != NULL)
        loc[0] = '\0';
    }
    
    loc = strrchr(path, '/');
    if(loc != NULL)
        loc[1] = '\0';
    
    return std::string(path);
}

std::string getResourcesDirectory()
{
    CFStringRef sRef = CFURLCopyPath(CFBundleCopyExecutableURL(CFBundleGetMainBundle()));
    char path[PATH_MAX];
    CFStringGetCString(sRef, path, sizeof(path), kCFStringEncodingASCII);
    
    char* loc = strrchr(path, '/');
    if(loc != NULL)
        loc[0] = '\0';
    
    loc = strrchr(path, '/');
    if(loc != NULL)
        loc[1] = '\0';
    
    return std::string(path) + "Resources/";
}
#endif