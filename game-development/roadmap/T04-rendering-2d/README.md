# T04 — Rendering Foundations & the GPU Pipeline
**Phase:** B | **Depends on:** T02, T03 | **Borrows from:** —
**Status:** stub | **Assignments:** A009–A013

## Why this topic, here
Rendering is where most people start and it is why most people never
understand it: they learn an API before they learn what the API is doing.
This topic does it the other way round. You write a **software
rasterizer** first (`A009`, `A010`) — you fill triangles, you interpolate
across them, you write your own depth buffer, you divide by w yourself.
Then you do the identical thing in OpenGL (`A011`) and every stage of the
pipeline is something you have already implemented by hand.

That detour costs about a week and it converts the entire rest of the
graphics track from memorisation into recognition.

The topic ends in 2D on purpose: a sprite renderer with batching, an
atlas, and a camera is what `T05`–`T07` need to become playable games.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Rasterization from First Principles | Framebuffers, line and triangle rasterization, barycentric coordinates, the z-buffer, fill rules and top-left tie-breaking | T02.S01 | 6 | A009, A010 | stub |
| S02 | Cameras & Projection | View matrix construction, perspective vs orthographic, clip space, the w-divide, NDC, viewport transform, depth precision and reversed-Z | T02.S02, S01 | 6 | A010 | stub |
| S03 | The GPU Pipeline & the Modern API Model | Vertex/fragment stages, buffers and vertex attributes, VAOs, uniforms, draw calls, pipeline state, and what "state machine" costs you | S02 | 6 | A011 | stub |
| S04 | Shaders & GLSL | The shader execution model, attributes/varyings/uniforms, samplers, precision, common shader math, and debugging shaders without a debugger | S03 | 5 | A011 | stub |
| S05 | Textures & Sampling | Formats and compression, filtering, mipmaps and why they exist, wrap modes, atlases and bleeding, sRGB and the linear workflow | S04 | 6 | A012 | stub |
| S06 | 2D Rendering in Practice | Sprite batching, draw-call reduction, sorting and layers, 2D cameras, tilemaps, culling, and pixel-perfect rendering | S05, T03.S04 | 6 | A012, A013 | stub |

## Exit criteria
- [ ] You can rasterize a triangle with correct barycentric interpolation and a fill rule that avoids double-shading shared edges.
- [ ] You can explain perspective-correct interpolation and show the artifact you get without it.
- [ ] You can write the full model → world → view → clip → NDC → screen chain and say what each matrix does and in what order it applies.
- [ ] You can explain why depth precision is non-uniform, and what reversed-Z with a floating-point buffer fixes.
- [ ] You can go from a `glDrawArrays` call backwards through every piece of state it depends on.
- [ ] You can explain what mipmaps solve, with the aliasing case written out, and why trilinear vs anisotropic differ.
- [ ] You can explain the sRGB/linear workflow and demonstrate a wrong-gamma image next to a right one.
- [ ] Your sprite renderer draws 10,000 sprites in a measured number of draw calls, and you can say what forces a batch break.

## Traps specific to this topic
- **Skipping the software rasterizer.** It feels like a detour. It is the
  single highest-value week in the graphics half of this roadmap.
- **Gamma.** Getting the linear workflow wrong early means every lighting
  result in `T08` is subtly wrong and you will tune around it forever.
- **Copy-pasting `learnopengl` without the matrices.** If you took `T02`
  seriously, build the MVP chain with *your own* math library, not GLM.
- **Chasing API versions.** OpenGL 4.5 core with DSA is fine for the whole
  of Phase B and C. Vulkan is `T08.S07`, not now.

## Primary sources for the whole topic
- `[TINYREND]` — the whole lesson series, for `S01`–`S02`
- `[SCRATCH]` — "Rasterization: a Practical Implementation" and the geometry sections
- `[LEARNOGL]` — "Getting Started" and "Lighting" sections, for `S03`–`S05`
- `[RTR4]` ch. 2 (graphics pipeline), ch. 4 (transforms), ch. 5.3 (sampling/aliasing), ch. 6 (texturing)
- `[FGED2]` ch. 5–6 — the projection and depth math done rigorously
