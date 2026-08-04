#ifndef NARWAL_THERMAL_ZONE_H
#define NARWAL_THERMAL_ZONE_H

#include <dirent.h>
#include <stddef.h>


// configs
#define NARWAL_THERMAL_ZONES_CONFIG_H

#define NARWAL_THERMAL_ZONE_PATH_MAX 100
#define NARWAL_THERMAL_ZONE_THERMAL_DIR "/sys/class/thermal/"

#define NARWAL_THERMAL_ZONE_TYPE_MAX 20
#define NARWAL_THERMAL_ZONE_TEMP_MAX 6
#define NARWAL_THERMAL_ZONE_TEMPERATURE_CELCIUS

#define PRIV_NARWAL_THERMAL_ZONES_OPT_NO_REWIND 1
#define PRIV_NARWAL_THERMAL_ZONES_OPT_REWIND 2

// end configs

/**
* @brief Represents a Linux thermal zone.
*
* This structure stores the information required to interact with a thermal
* zone through the Narwal Thermal Zones interface.
*/
typedef struct {
  char   path[NARWAL_THERMAL_ZONE_PATH_MAX];  /**< Absolute path to the corresponding thermal zone directory. */
  size_t path_len;                            /**< Length of @ref path, excluding the null terminator. */
  char   type[NARWAL_THERMAL_ZONE_TYPE_MAX];  /**< Type or name reported by the thermal zone. */
  size_t type_len;                            /**< Length of @ref type, excluding the null terminator. */
  int    error;                               /**< Error code associated with the last operation. */
} NarwalThermalZone;


//private functions
int priv_narwal_thermal_zones_fill_next(NarwalThermalZone *tz_p, unsigned int opts);
//end private functions



float narwal_thermal_zones_get_temp(NarwalThermalZone *tz_p);

/*
 * @brief Retrive the next thermal zone available on the system
 *
 * Retrive the next thermal zone available on the system
 *
 * On each successful call, this function clears the structure pointed to by
 * "tz_p" and fills it with information about the next available thermal zone.
 * Successive calls iterate through all thermal zones detected on the system.
 *
 * This function is a wrrapper of the priv_narwal_thermal_zones_fill_next function
 *
 * @param tz_p  Pointer to the structure in which the thermal zone information
 *              will be stored.
 *
 * @return A status code indicating whether a thermal zone was retrieved,
 *         whether the iteration has finished, or whether an error occurred.
 *
 * @retval NARWAL_THERMAL_ZONES_OPEN_ERR Indicates an error when trying to opne
 * the /sys/class/thermal/ directory 
*/
#define narwal_thermal_zones_fill_next(tz_p)  priv_narwal_thermal_zones_fill_next((tz_p), PRIV_NARWAL_THERMAL_ZONES_OPT_NO_REWIND)


/**
* @brief Retrieve the thermal zone corresponding to the provided type string
*
* Retrieve the thermal zone corresponding to the provided type string.
* This function clears the structure pointed to by "tz_p", so it is safe
* to use with a previously initialized thermal zone.
*
* In case of an error, a negative value is returned indicating the error.
* The contents of the thermal zone pointed to by "tz_p" may be left in an
* undefined state.
*
* @param tz_p Pointer to the structure where the thermal zone information
* will be stored.
*
* @param type_str_p String containing the type name of the thermal zone.
*
* @return An event code. If the returned value is negative, an error has
* occurred.
*
* @retval NARWAL_THERMAL_ZONES_SUCCESS The thermal zone structure was filled
* successfully.
* @retval NARWAL_THERMAL_ZONES_NULL_PTR Either "tz_p" or "type_str_p" is NULL.
* @retval NARWAL_THERMAL_ZONES_OPEN_ERR An error occurred while trying to open
* the thermal zone directory /sys/class/thermal/.
* @retval NARWAL_THERMAL_ZONES_READ_ERR An error occurred while trying to read
* the type file of a thermal zone.
*/
int narwal_thermal_zones_fill_by_type(NarwalThermalZone *tz_p, const char *type_str_p);


/**
* @brief Return the type name of the provided thermal zone
*
* Return the type name of the thermal zone pointed to by "tz_p".
*
* On a successful call, this function returns a pointer to the type string
* stored in the provided thermal zone structure.
*
* If an error occurs, NULL is returned and the "error" field of the structure
* pointed to by "tz_p" is set with the corresponding error code.
*
* @param tz_p Pointer to the thermal zone whose type will be retrieved.
*
* @return A pointer to the thermal zone type string on success, or NULL if
* an error occurs.
*/
char *narwal_thermal_zones_get_type(NarwalThermalZone *tz_p);

enum NARWAL_THERMAL_ZONE_RC {
  NARWAL_THERMAL_ZONE_ERR,
  NARWAL_THERMAL_ZONE_SUCESS,
  // there are no more thermal zones left
  NARWAL_THERMAL_ZONE_NO_MORE_TZ,
  NARWAL_THERMAL_ZONE_TZ_DOES_NOT_EXIST,
};

enum NARWAL_THERMAL_ZONE_ERRS {
  NARWAL_THERMAL_ZONE_INVALID_PATH,
};


#define NARWAL_THERMAL_ZONES_NO_MORE_TZ       1
#define NARWAL_THERMAL_ZONES_SUCCESS          0
#define NARWAL_THERMAL_ZONES_OPEN_ERR        -1
#define NARWAL_THERMAL_ZONES_READ_ERR        -2
#define NARWAL_THERMAL_ZONES_KEY_NOT_FOUND   -3
#define NARWAL_THERMAL_ZONES_TO_LARGE_ENTRY  -4
#define NARWAL_THERMAL_ZONES_NULL_BUF        -5
#define NARWAL_THERMAL_ZONES_NULL_PTR        -6
#define NARWAL_THERMAL_ZONES_SEEK_RESET_ERR  -7



#endif
