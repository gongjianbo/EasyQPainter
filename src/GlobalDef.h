#pragma once

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
#define GetTextWidth(fm, text) fm.width(text)
#else
#define GetTextWidth(fm, text) fm.horizontalAdvance(text)
#endif
#define GetTextHeight(fm) fm.height()

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
#define GetMousePos(evt) evt->pos()
#define GetMouseDelta(evt) evt->delta()
#else
#define GetMousePos(evt) evt->position().toPoint()
#define GetMouseDelta(evt) evt->angleDelta().y()
#endif
