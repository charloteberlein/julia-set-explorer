# julia-set-explorer
A mini app to visualise Julia sets based on the mouse position.
- inspired by this video: https://youtu.be/nr8biZfSZ3Y?si=WUcSGoKV80Fr1rsS (6:47)
### Optimisations
- Added precomputed colour lookup table to avoid repeated arithmetic
- Flattened 2D loops to use 1D index maths instead, which makes vectorisation easier
- Used a raw pixel buffer to update the final RGBA data directly
- Enabled compiler autovectorisation (SIMD) by compiling as a release build with -Ofast and -march=native (biggest performance boost)
- Reduced unnecessary branching inside loops
- Direct texture updates from raw pixel buffer
- Parallelised using OpenMP
## License
No license granted. All rights reserved.
