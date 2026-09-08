# T08 — 3D Rendering: Lighting, Shading & Modern Pipelines
**Phase:** C | **Depends on:** T04 | **Borrows from:** —
**Status:** stub | **Assignments:** A025–A029

## Why this topic, here
Phase C starts here because 3D rendering has the deepest theory stack of
anything left, and because `T09` (animation) is unviewable without it.

The order inside the topic is deliberate and slightly unusual: the
**physics of light comes before the shading models**. Learning
Cook-Torrance as a formula to copy is a common and expensive mistake —
the roughness parameter stops being magic the moment you know what a
microfacet distribution is. One unit of radiometry saves a year of
tweaking numbers until it looks right.

It ends with explicit APIs (`S07`, Vulkan). Not because you need Vulkan
to make games — you do not — but because the mental model it forces
(explicit synchronization, descriptor layouts, render graphs) is what
modern engine rendering code looks like.

## Subtopics

| id | subtopic | scope in one line | depends on | units | assignments | status |
|---|---|---|---|---:|---|---|
| S01 | Light & Materials: The Physics | Radiometry (radiance, irradiance, the solid angle), the rendering equation, BRDFs, energy conservation, the linear/HDR workflow | T04.S05 | 6 | A026 | stub |
| S02 | Shading Models | Lambert, Phong and Blinn-Phong, then Cook-Torrance: the D/F/G terms, GGX, metallic-roughness vs specular-glossiness | S01 | 7 | A025, A026 | stub |
| S03 | Normal & Parallax Mapping | Tangent space and how it is built, normal map conventions, handedness/green-channel bugs, parallax occlusion mapping | S02, T02.S02 | 5 | A025 | stub |
| S04 | Shadows | Shadow mapping, the projection and bias problem, acne vs peter-panning, PCF and PCSS, cascades, point-light shadows, alternatives | S02 | 6 | A027 | stub |
| S05 | Ambient, IBL & Global Illumination | The split-sum approximation, irradiance and prefiltered environment maps, SSAO, light probes and lightmaps, an honest overview of RT/GI options | S01, S02 | 6 | A026, A028 | stub |
| S06 | Deferred, Forward+ & Post-Processing | G-buffer layouts and their cost, tiled/clustered forward, transparency's problem, bloom, tonemapping (ACES), exposure, AA (MSAA/FXAA/TAA) | S02, S05 | 7 | A028 | stub |
| S07 | Modern Explicit APIs | The Vulkan/D3D12 model: queues, command buffers, descriptors, memory, pipeline barriers and synchronization, render graphs, bindless | S06 | 7 | A029 | stub |

## Exit criteria
- [ ] You can state the rendering equation and say what each term is, in words, without symbols.
- [ ] You can explain what "energy conservation" means for a BRDF and demonstrate a material that violates it.
- [ ] You can explain each of D, F, and G in Cook-Torrance in terms of what physical effect it models.
- [ ] You can explain why metals have no diffuse term.
- [ ] You can construct a TBN matrix and explain the two most common normal-map bugs and their visual signature.
- [ ] You can explain shadow acne in terms of depth-map resolution and slope, and say why normal-offset bias beats constant bias.
- [ ] You can list a G-buffer layout with per-channel bit budgets and justify each choice.
- [ ] You can explain why tonemapping is required and what happens without it in an HDR pipeline.
- [ ] You can explain what a pipeline barrier does and give a concrete case where omitting one produces a race.

## Traps specific to this topic
- **Copying a PBR shader.** It will look approximately right and you will
  learn nothing. `A026` requires deriving the terms and a furnace test.
- **Skipping the linear workflow.** If `T04.S05` was rushed, everything
  here is wrong by a gamma curve and you will fight it in every scene.
- **Vulkan too early.** `S07` is last for a reason. Vulkan before you can
  articulate what the driver was doing for you in OpenGL is 2000 lines of
  boilerplate that teaches nothing.
- **Believing "PBR" means "photorealistic".** It means "energy-consistent
  and parameterized by material properties". Stylized games use it too.

## Primary sources for the whole topic
- `[RTR4]` ch. 5, 7 (shadows), 8 (light & color), 9 (physically based shading), 10, 12 (post), 20 (efficient shading) — the spine
- `[PBRT]` ch. 4 (radiometry), 9 (reflection models) — free, and the rigorous version of `S01`–`S02`
- `[LEARNOGL]` — the PBR and Advanced Lighting sections, as the implementation companion
- `[FGED2]` — for the shadow and projection math
- `[VKGUIDE]` then `[VKSPEC]` — for `S07`
- `[GPUGEMS]` — specific chapters cited per unit
