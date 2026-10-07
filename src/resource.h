// BaifanYu (白饭鱼) for Windows — resource identifiers.
// Shared between the C++ sources and app.rc.
#ifndef BAIFANYU_RESOURCE_H
#define BAIFANYU_RESOURCE_H

// Application icon (multi-size .ico, also used for the notification-area icon).
#define IDI_APPICON 101

// Skin artwork.  Each skin owns a contiguous block:
//   base + 0  -> standing art ("no expression")
//   base + 1..6 -> the six faces, in PetSkin::faceOrder order
#define IDR_SKIN_BASIN_BASE 210
#define IDR_SKIN_MAID_BASE  230
#define SKIN_ID_STRIDE      20

// Rubber-duck squeaks.
#define IDR_DUCK1 310
#define IDR_DUCK2 311
#define IDR_DUCK3 312

#endif  // BAIFANYU_RESOURCE_H
