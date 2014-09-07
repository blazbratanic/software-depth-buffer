#ifndef LIKELY_H
#define LIKELY_H

#define LIKELY __attribute__((hot))
#define UNLIKELY __attribute__((cold))

#endif
