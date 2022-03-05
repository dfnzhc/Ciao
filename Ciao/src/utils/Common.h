#pragma once

namespace Ciao
{
    constexpr float Pi				= 3.14159265358979323846f;
    constexpr float Inv_Pi			= 0.31830988618379067154f;
    constexpr float Inv_TwoPi		= 0.15915494309189533577f;
    constexpr float Inv_FourPi		= 0.07957747154594766788f;
    constexpr float Sqrt_Two		= 1.41421356237309504880f;
    constexpr float Inv_Sqrt_Two	= 0.70710678118654752440f;

    constexpr float Epsilon			= 1e-3f;

    #define _USE_MATH_DEFINES
    #include <cmath>
    #include <glm/glm.hpp>
    #include <glm/ext.hpp>

    using glm::mat4;
    using glm::mat3;
    using glm::vec2;
    using glm::vec3;
    using glm::vec4;
    using glm::ivec2;

    constexpr std::string ResourceDir(const std::string& s) { return std::string("Assets/").append(s); }
    #define Res(s) ResourceDir(s).c_str()

    //// Convert radians to degrees
    inline float radToDeg(float value) { return value * (180.0f / Pi); }

    /// Convert degrees to radians
    inline float degToRad(float value) { return value * (Pi / 180.0f); }

    template <typename T>
    inline void MergeVectors(std::vector<T>& v1, const std::vector<T>& v2)
    {
        v1.insert(v1.end(), v2.begin(), v2.end());
    }

    // From https://stackoverflow.com/a/64152990/1182653
    // Delete a list of items from std::vector with indices in 'selection'
    template <class T, class Index = int>
    inline void EraseSelected(std::vector<T>& v, const std::vector<Index>& selection)
    // e.g., eraseSelected({1, 2, 3, 4, 5}, {1, 3})  ->   {1, 3, 5}
    //                         ^     ^    2 and 4 get deleted
    {
        // cut off the elements moved to the end of the vector by std::stable_partition
        v.resize(std::distance(
            v.begin(),
            // the stable_partition moves any element whose index is in 'selection' to the end
            std::stable_partition(v.begin(), v.end(),
                                  [&selection, &v](const T& item)
                                  {
                                      return !std::binary_search(
                                          selection.begin(), selection.end(),
                                          /* std::distance(std::find(v.begin(), v.end(), item), v.begin()) - if you don't like the pointer arithmetic below */
                                          static_cast<Index>(static_cast<const T*>(&item) - &v[0]));
                                  })));
    }

    inline bool CheckFileExist(const std::string& fileName)
    {
        //return std::filesystem::exists(fileName);

        // fast check file if exists
        // https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
        struct stat buffer;
        return (stat(fileName.c_str(), &buffer) == 0);
    }
    
    inline std::string RawFilename(const std::string& f)
    {
        return f.substr(0, f.find_last_of("."));
    }

    enum SuffixType
    {
        CubeMap = 0,
        
    };

    const std::string suffixs[] = {"_Cross.hdr",};

    inline std::string GetSuffix(SuffixType s)
    {
        return suffixs[s];
    }
}
