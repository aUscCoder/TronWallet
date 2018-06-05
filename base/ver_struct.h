// Author: WuJian
#pragma once
#include <string>
#include <stdlib.h> // for atoi in Mac

struct VerInfo
{
    std::uint32_t dwVer[4];

    VerInfo()
    {
        dwVer[0] = dwVer[1] = dwVer[2] = dwVer[3] = 0;
    }
    VerInfo(std::uint32_t dw0, std::uint32_t dw1, std::uint32_t dw2, std::uint32_t dw3)
    {
        dwVer[0] = dw0;
        dwVer[1] = dw1;
        dwVer[2] = dw2;
        dwVer[3] = dw3;
    }

  VerInfo(const char *str)
  {
    FromStr(str);
  }

    std::string ToStr() const
    {
        char sz[128];
        sprintf(sz, "%d.%d.%d.%d", dwVer[0], dwVer[1], dwVer[2], dwVer[3]);
        return sz;
    }


    bool FromStr(const char* lpszVerText)
    {
        int iDotCount = 0;
        int iLastDotPos = -1;
        int i = 0;
        for (; lpszVerText[i] != '\0'; i++)
        {
            if (lpszVerText[i] <= '9' && lpszVerText[i] >= '0')
            {
                continue;
            }
            else if (lpszVerText[i] == L'.')
            {
                if (i - iLastDotPos > 6 || i - iLastDotPos < 2) //两个.之间必须有数字，且小于65535
                    return false;
                int iPart = atoi(lpszVerText + iLastDotPos + 1);
                if (iPart > 0xFFFF)
                    return false;
                dwVer[iDotCount] = iPart;
                iLastDotPos = i;
                iDotCount++;
                if (iDotCount >= 4)
                    return false;
            }
            else
            {
                return false;
            }
        }
        if (i == 0)
            return false;
        if (i - iLastDotPos > 6 || i - iLastDotPos < 2)//最后一个.后面必须有数字，且小于65535
            return false;
        int iPart = atoi(lpszVerText + iLastDotPos + 1);
        if (iPart > 0xFFFF)
            return false;
        dwVer[iDotCount] = iPart;
        for (int j = iDotCount + 1; j < 4; j++)
            dwVer[j] = 0;
        return true;
    }

  bool operator < (const char *v) const { return (*this) < VerInfo(v); }
  bool operator > (const char *v) const { return (*this) > VerInfo(v); }
  bool operator <= (const char *v) const { return (*this) <= VerInfo(v); }
  bool operator >= (const char *v) const { return (*this) >= VerInfo(v); }
  bool operator == (const char *v) const { return (*this) == VerInfo(v); }
  bool operator != (const char *v) const { return (*this) != VerInfo(v); }

    bool operator < (const VerInfo &v) const
    {
        for (int i = 0; i < 4; i++)
        {
            if (dwVer[i] < v.dwVer[i])
                return true;
            else if (dwVer[i] > v.dwVer[i])
                return false;
        }
        return false;
    }

    bool operator > (const VerInfo &v) const
    {
        for (int i = 0; i < 4; i++)
        {
            if (dwVer[i] > v.dwVer[i])
                return true;
            else if (dwVer[i] < v.dwVer[i])
                return false;
        }
        return false;
    }

    bool operator <= (const VerInfo &v) const
    {
        for (int i = 0; i < 4; i++)
        {
            if (dwVer[i] < v.dwVer[i])
                return true;
            else if (dwVer[i] > v.dwVer[i])
                return false;
        }
        return true;
    }

    bool operator >= (const VerInfo &v) const
    {
        for (int i = 0; i < 4; i++)
        {
            if (dwVer[i] > v.dwVer[i])
                return true;
            else if (dwVer[i] < v.dwVer[i])
                return false;
        }
        return true;
    }

    bool operator == (const VerInfo &v) const // NOLINT (3.3.2 真的需要啊)
    {
        for (int i = 0; i < 4; i++)
            if (dwVer[i] != v.dwVer[i])
                return false;
        return true;
    }

    bool operator != (const VerInfo &v) const // NOLINT (3.3.2 真的需要啊)
    {
        return !operator == (v);
    }

    bool EqualZero() const
    {
        for (int i = 0; i < 4; i++)
            if (dwVer[i] != 0)
                return false;
        return true;
    }
};
