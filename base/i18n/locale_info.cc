#include "base/i18n/locale_info.h"

#include "unicode/ulocdata.h"

namespace base {

namespace i18n {

std::vector<uint16_t> GetLocaleExemplarSet(const std::string& locale_name) {
  UErrorCode error = U_ZERO_ERROR;
  std::vector<uint16_t> resultCharSets;
  resultCharSets.reserve(128);
  resultCharSets.resize(127);
  ULocaleData* uld = ulocdata_open(locale_name.c_str(), &error);
  if (uld && U_SUCCESS(error)) {
    USet *set = ulocdata_getExemplarSet(uld, NULL, 0, ULOCDATA_ES_STANDARD, &error);
    ulocdata_close(uld);
    if (set && U_SUCCESS(error)) {
      int need_size = uset_serialize(set, &resultCharSets[0], 127, &error);
      if (U_SUCCESS(error)) {
        resultCharSets.resize(need_size);
      } else if (error == U_BUFFER_OVERFLOW_ERROR) {
        resultCharSets.reserve(need_size + 1);
        resultCharSets.resize(need_size);
        uset_serialize(set, &resultCharSets[0], need_size, &error);
      } else {
        resultCharSets.clear();
      }
    }
  }
  return resultCharSets;
}



}
}
