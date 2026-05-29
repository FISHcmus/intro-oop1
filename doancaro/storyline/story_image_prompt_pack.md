# Story Image Prompt Pack

This file is the source of truth for story-art generation.

Do not call image generation until these prompts are reviewed.

## Why the previous batch failed

- It drifted into dark mythic realism instead of bright `wuxia painting cartoon`.
- It forgot that several intro beats happen in present-day southern Vietnam, not a fully ancient city.
- It used portrait-glamour compositions that do not fit the game's wide story frame.
- It exposed too many hands and fingers in important foreground positions, which makes bad anatomy more obvious.
- It kept reusing one heroine angle instead of varying mood, costume, hairstyle, and staging by scene.

## Technical target

- Main story illustrations: wide banner composition, designed for a crop-safe frame around `540x210`.
- Recommended generation size: at least `2048x800`.
- Keep the main subject inside the center `70%` width and center `65%` height.
- Avoid important faces, hands, or beast heads at the bottom edge.
- If hands appear, keep them simple: holding a book, tucked in sleeves, resting on railing, or silhouette gesture only.
- No image should look like a poster, cover art, or glamour portrait. Each one is a story panel.

## Global style lock

- Bright `wuxia painting cartoon`
- 2D hand-painted look
- Ink linework plus watercolor / gouache wash
- Child-friendly adventure tone
- Vietnamese folklore motifs: tiled roofs, lotus ponds, river mist, bamboo, mountain forms, decorative clouds
- Soft paper texture
- Flatter painted values, not cinematic realism
- Faces stylized and gentle, not glossy anime, not hyper-real

## Global avoid list

- No dark fantasy oil-painting realism
- No grim apocalypse palette
- No generic ancient-Chinese palace city unless the beat specifically calls for mythic court imagery
- No Western medieval armor or castle shapes
- No horror faces, gore, corpse imagery, or trauma porn
- No giant foreground hands with spread fingers
- No full ancient city backdrop on pages that happen in modern-day Saigon
- No random skyline if the scene is clearly historical or mythic

## Character continuity

### Cô Sử Tiên / Mỵ Nương

- Vietnamese immortal woman who looks around `22-26`
- Oval face, calm eyes, refined but human, not doll-like
- Hair can change by scene: half-up scholar style, low bun, wind-loosened braid, ceremonial crown, rain hood
- Costume family stays consistent: teal / jade / ivory / muted gold, scholar-princess silhouette, flowing sleeves
- In present-day scenes, clothing can be a modernized scholar coat or áo dài-inspired outer layer, not pure court costume
- Emotional arc across the intro: observant -> uneasy -> confessional -> burdened -> mythic memory -> judging -> regret -> resolve

### Sơn Tinh

- Noble mountain-warrior spirit
- Earth, roots, stone, pine, warm gold-brown palette
- Protective, steady, not arrogant

### Thủy Tinh

- Water-lord spirit
- Turquoise, indigo, foam-white palette
- Dangerous and proud, but still stylized for younger audiences
- Menacing through scale and motion, not horror anatomy

## Shared prompt suffix

Append this intent mentally to every prompt:

`bright child-friendly wuxia painting cartoon, 2D hand-painted story illustration, watercolor paper texture, readable shapes, Vietnamese folklore atmosphere, flatter values, no photorealism, no glossy anime, no dark fantasy, no text, no watermark`

## Prompt list

## Intro sequence

### `assets/images/story/intro/intro-page-01-wuxia-v4.png`

Beat: Mỵ Nương in present-day Saigon, morning coffee, watching ordinary people rush to work after a night rain.

Prompt:

```text
Wide story-banner illustration for a Vietnamese folklore game. Present-day Saigon after a night rain, seen from a small cafe balcony above a canal-side street. Cô Sử Tiên, a youthful Vietnamese immortal woman, stands quietly with a closed book and a ceramic coffee cup nearby, watching office workers, scooters, umbrellas, and morning light. She wears a modernized teal-and-ivory scholar coat inspired by áo dài and giao lĩnh, with simple gold trim. Hair in a soft half-up style with ribbon, gentle curious expression, no dramatic hand pose. The city must feel modern southern Vietnamese: cafe awning, wet pavement, canal bridge, trees, low tiled roofs mixed with distant skyline, not an ancient capital. Warm sunrise gold, jade, peach, sky blue, child-friendly wonder, elegant decorative clouds, wide composition with breathing room for text.
```

### `assets/images/story/intro/intro-page-02-wuxia-v4.png`

Beat: Beneath ordinary city life, the drains and ground begin to tremble.

Prompt:

```text
Wide story-banner illustration. Present-day Vietnamese street corner in the rainy season, with a storm drain trembling as water bubbles up too early. In the foreground, a worried mother carrying medicine, a street vendor lifting goods, and a child under a raincoat show ordinary life under pressure. Cô Sử Tiên stands under a modest shop awning in a darker teal travel coat, low bun hairstyle, looking down with unease as if hearing something inside the earth. The mood is uneasy but still suitable for children: no disaster spectacle yet, only a strange warning. Modern Saigon cues only: utility wires, scooters, concrete curb, small storefront, wet asphalt. Bright misty palette, watercolor wash, no horror.
```

### `assets/images/story/intro/intro-page-03-wuxia-v4.png`

Beat: She reveals her father is Lạc Long Quân, now dispersed into the land.

Prompt:

```text
Wide story-banner illustration. Mythic revelation scene: Cô Sử Tiên stands on a hill above a river plain as the geography itself softly forms the presence of Lạc Long Quân in mountains, river deltas, clouds, and wind. His form should be subtle and majestic, built from land and weather, not a literal giant monster. She now wears an older, more ceremonial jade-and-gold robe with long sleeves and a small crown-pin, hair braided and partly lifted by wind. Her expression is solemn, intimate, almost saying "this is family." The landscape should feel Vietnamese: layered hills, broad river, fertile delta, warm wind, birds, mist. Bright sacred atmosphere, child-friendly myth, painted clouds and flowing earth lines.
```

### `assets/images/story/intro/intro-page-04-wuxia-v4.png`

Beat: Four thousand years of changing names, watching kingdoms rise and fall, hiding behind writing.

Prompt:

```text
Wide story-banner illustration. Storybook montage scene: Cô Sử Tiên walking through shifting eras of Vietnamese history while carrying scrolls and books, her figure repeated in gentle echoes from left to right. Behind her, the world changes through stylized layers: bronze drums, old wooden halls, river boats, scholar desks, market roofs, war banners far away, then fading into modern streets. She wears a traveling scholar robe, darker teal with muted gold, hair in a practical high knot, expression tired and self-accusing rather than glamorous. The image should read as time passing and witnesshood, not a busy history poster. Clean focal path, warm paper tones, poetic and child-readable.
```

### `assets/images/story/intro/intro-page-05-wuxia-v4.png`

Beat: From the first wound came the two forces: mountain and water.

Prompt:

```text
Wide story-banner illustration. Mythic split scene in bright wuxia painting cartoon style: the land opens into a luminous ancient wound, and from it arise two opposite powers in symbolic form. On one side, Sơn Tinh appears through mountain, roots, stone, pine, and earth-colored energy. On the other, Thủy Tinh appears through waves, foam, river dragons, and blue water currents. Cô Sử Tiên is a small central witness in pale ceremonial robes, hair blown back, looking stricken. The split must feel grand but not terrifying: decorative energy, swirling ink-water shapes, clean silhouettes, no apocalyptic gore, no horror. This is a foundational myth image for children.
```

### `assets/images/story/intro/intro-page-06-wuxia-v4.png`

Beat: The old judgment scene with the wedding offerings.

Prompt:

```text
Wide story-banner illustration. Mythic court of judgment under open sky, not a closed palace. Cô Sử Tiên sits or stands in a dignified judging role, wearing the richest version of her costume: gold, jade, ivory ceremonial layers, hair in an elegant crown-bun. Before her are Sơn Tinh and Thủy Tinh as rival suitors, both stylized and noble, not monstrous. The three offerings must be clearly present in symbolic readable form: Voi chín ngà, Gà chín cựa, Ngựa chín hồng mao. Composition should show the tension of choice and hidden bias. Use warm gold light and a balanced wide staging, like a classical painted folktale panel, not a war scene.
```

### `assets/images/story/intro/intro-page-07-wuxia-v4.png`

Beat: Sơn Tinh arrives first, Thủy Tinh arrives too late, and the old resentment begins.

Prompt:

```text
Wide story-banner illustration. First flood-war beginning. Sơn Tinh ascends with the offerings and mountain power while Thủy Tinh arrives a single moment too late, water rearing behind him. Cô Sử Tiên stands on a high terrace or cliff, looking guilty as the first great clash begins below. Use strong directional motion: rising earth and roots against rising water and storm. Child-friendly epic tone: dramatic but not violent, no drowning bodies, no horror. Her costume is wind-torn and more restrained than the court scene, hair partially loosened, face full of regret. Bright storm palette with teal, gold, blue-gray, and flashes of foam-white.
```

### `assets/images/story/intro/intro-page-08-wuxia-v4.png`

Beat: Present-day vow. This time she will not only write, she will join the fight.

Prompt:

```text
Wide story-banner illustration. Present-day southern city at dusk before a coming flood, but with hope. Cô Sử Tiên stands on a bridge or rooftop looking down at a faint glowing dragon-vein line beneath wet streets and canal water. She now wears a practical battle-travel version of her scholar clothing: layered teal coat, tied sleeves, rain cloak folded back, hair in a determined braid. One simple hand grips a scroll case or book strap; avoid open fingers. Her face is resolved, no longer passive. The city must clearly be modern southern Vietnam with mixed low roofs, bridge, canal, distant towers, warning clouds, and subtle mythic glow under the earth. Adventurous, brave, luminous, kid-friendly.
```

## Set intros

### `assets/images/story/set-intro/set1-wuxia-v1.png`

Beat: Early flood warning in the city.

Prompt:

```text
Wide story-banner illustration. Set 1 intro: early-season rainwater rising from storm drains into a busy southern Vietnamese street. Ticket seller, commuters lifting pant legs, small shop awnings, scooters parked aside, everyone surprised but still active. Cô Sử Tiên stands near the edge pointing Sơn Tinh toward the weak ground beneath the city. Present-day urban Vietnam, not an ancient town. Bright rain palette, reflective puddles, playful but tense movement, child-friendly adventure.
```

### `assets/images/story/set-intro/set2-wuxia-v1.png`

Beat: Long An, the dragon vein cracking, Sơn Tinh rides Voi chín ngà.

Prompt:

```text
Wide story-banner illustration. Set 2 intro in Long An wetlands and rice fields. Sơn Tinh rides Voi chín ngà across cracked earth where blue water is drilling upward from a dragon-vein fault. Cô Sử Tiên, wearing a travel robe and wind scarf, points toward the fracture with grave urgency. The landscape should be southern plains, canals, reeds, rice fields, and low horizon, not mountain China. The elephant is noble and stylized, huge but friendly in silhouette. Bright daylight, green and gold palette, mythic but grounded in Vietnam.
```

### `assets/images/story/set-intro/set3-wuxia-v1.png`

Beat: The sea rises into a wall to force Saigon to bow.

Prompt:

```text
Wide story-banner illustration. Set 3 intro: an enormous wall of sea rises beyond modern Saigon and canal districts, towering over bridges and rooftops. Thủy Tinh's presence is visible inside the wave as a proud water-lord silhouette, not horror. Cô Sử Tiên and Sơn Tinh stand together on high ground preparing to resist. The city must read as modern southern Vietnam with skyline in the distance and low neighborhoods in the foreground. Bright but intense storm colors, huge scale, child-friendly epic courage.
```

### `assets/images/story/set-intro/finalboss-wuxia-v1.png`

Beat: Thủy Tinh appears in full nine-headed form.

Prompt:

```text
Wide story-banner illustration. Final boss intro: Thủy Tinh revealed in full as a nine-headed water titan rising from sea and storm, with each head stylized like a dragon-serpent of flood season, not grotesque. Sơn Tinh faces him on lifted earth while Cô Sử Tiên stands behind with scroll and vow. The three linh vật can appear faintly in the sky or around the battlefield as signs of support. Grand heroic confrontation, strong clean silhouettes, bright mythic storm colors, suitable for children.
```

## Set win screens

### `assets/images/story/set-win/set1-wuxia-v1.png`

Beat: The first wave is stopped, but the enemy has learned the southern dragon vein.

Prompt:

```text
Wide story-banner illustration. Set 1 win: city water receding from streets while people begin cleaning up and helping each other. Cô Sử Tiên kneels or leans to touch the wet ground, sensing a faint glowing dragon-vein rhythm below. Her expression is relieved but worried. Present-day southern Vietnamese city, bright clearing sky after rain, hopeful mood with a hidden warning.
```

### `assets/images/story/set-win/set2-wuxia-v1.png`

Beat: The dragon vein is held, but cracked. Gà chín cựa answers from dawn.

Prompt:

```text
Wide story-banner illustration. Set 2 win: dawn over cracked but surviving fields and wetlands. A luminous fault line glows softly in the earth, and Gà chín cựa appears on a high mound or shrine roof, crowing toward first light. Cô Sử Tiên watches with cautious hope, hair loosened by the long battle, pale sunrise cloak over her scholar robe. This should feel like night finally fearing dawn, not total victory. Bright sunrise colors, elegant rooster silhouette, child-friendly myth.
```

### `assets/images/story/set-win/set3-wuxia-v1.png`

Beat: The wall of sea is broken, and Ngựa chín hồng mao descends.

Prompt:

```text
Wide story-banner illustration. Set 3 win: shattered water-wall falling into sparkling harmless sheets while Ngựa chín hồng mao descends from cloud and sunlight, fiery mane bright red-gold but friendly. Sơn Tinh stands triumphant yet calm. Cô Sử Tiên looks upward in awe, wearing a battle-darkened but still elegant teal outfit. Modern city below survives. Big sense of breakthrough and gathering final courage.
```

### `assets/images/story/set-win/finalboss-wuxia-v1.png`

Beat: Thủy Tinh retreats into the sea. Hatred remains, but tonight the land stands.

Prompt:

```text
Wide story-banner illustration. Final boss win: the nine-headed water titan dissolves back into waves under moonlit blue and warm lantern gold, not dying in gore, just forced to retreat. Sơn Tinh, Cô Sử Tiên, and the three linh vật stand in victorious silhouette on renewed earth. In the distance, homes, boats, and riverbanks remain safe. Triumphant but reflective, suitable for children, bright mythic night rather than horror night.
```

## Set lose screens

### `assets/images/story/set-lose/set1-wuxia-v1.png`

Beat: Water rushes into homes; the city is revealed as fragile.

Prompt:

```text
Wide story-banner illustration. Set 1 lose: shallow floodwater rushing into a small southern Vietnamese home and shopfront, lantern light out, cooking fire gone cold, family gathering belongings onto a high table. Cô Sử Tiên watches in pain from the doorway or street edge. Keep it sorrowful but safe for children: no drowning, no panic horror, only fragile everyday life overwhelmed by water. Bright rain-blue palette with warm memory of home.
```

### `assets/images/story/set-lose/set2-wuxia-v1.png`

Beat: The dragon vein breaks; fields and villages become nameless mud.

Prompt:

```text
Wide story-banner illustration. Set 2 lose: cracked earth collapses into muddy flood across fields, dikes, and village edges in Long An. The glowing dragon-vein line shatters under water. Cô Sử Tiên stands on surviving high ground with grief and urgency, sleeves whipped by wind. Southern plain landscape only: canals, fields, low trees, huts. Sad but readable folktale image, no gore or terror.
```

### `assets/images/story/set-lose/set3-wuxia-v1.png`

Beat: The sea-wall becomes a grave for the city.

Prompt:

```text
Wide story-banner illustration. Set 3 lose: modern Saigon and canal neighborhoods nearly swallowed behind a towering sea-wall, with rooftops and bridges disappearing into mist and water. The mood is tragic and awed, not horror. Cô Sử Tiên appears small against the scale, face shocked by how helpless people can feel. Strong silhouette of the city losing ground, but keep colors readable and painterly, not grim blackness.
```

### `assets/images/story/set-lose/finalboss-wuxia-v1.png`

Beat: The old judgment breaks; land returns to mud and water.

Prompt:

```text
Wide story-banner illustration. Final boss lose: a symbolic vision of names, houses, fields, lullabies, and history sinking back into primeval marsh. Thủy Tinh's nine-headed form dominates the sky-water horizon while the land beneath softens into ancient brine. Do this as poetic tragedy, not nightmare horror. No corpses, no gore. Cô Sử Tiên's book slips from her grasp or hangs at her side as she realizes the old decree has failed. Mythic, sorrowful, child-safe.
```

## Unlock scenes

### `assets/images/story/unlock/voi-wuxia-v1.png`

Beat: Voi chín ngà returns, linked to the memory of Hai Bà Trưng.

Prompt:

```text
Wide story-banner illustration. Unlock scene for Voi chín ngà. The great white-gray nine-tusk elephant rises from earth and mist like a moving hill, noble and protective. Behind or within the background, subtle mural-like echoes of Hai Bà Trưng and war banners suggest its remembered awakening, but the elephant remains the focus. Cô Sử Tiên stands with reverence, sleeves gathered, looking up. Child-friendly sacred power, bright earth and ivory palette.
```

### `assets/images/story/unlock/ga-wuxia-v1.png`

Beat: Gà chín cựa answers from the age of Cổ Loa.

Prompt:

```text
Wide story-banner illustration. Unlock scene for Gà chín cựa. A proud nine-spurred rooster with white head accents stands atop a spiral rampart or high roof at dawn, crowing toward the sky. Behind it, stylized Cổ Loa ramparts and a gentle memory of Kim Quy and the old citadel appear as painted motifs, not crowded literal history. Cô Sử Tiên watches from below, hopeful. Bright dawn gold and celadon, heroic and playful.
```

### `assets/images/story/unlock/ngua-wuxia-v1.png`

Beat: Ngựa chín hồng mao returns through the memory of Phù Đổng.

Prompt:

```text
Wide story-banner illustration. Unlock scene for Ngựa chín hồng mao. A red-maned mythical horse descends from clouds with wind and warm firelight, swift and noble, echoing the legend of Phù Đổng. In the background, a stylized child-hero silhouette and mountain trace can appear like a mural memory. Cô Sử Tiên braces against the wind, awed but smiling. Energetic sky composition, bright red-gold, blue, and ivory palette, child-friendly wuxia adventure.
```

## Epilogue

### `assets/images/story/epilogue/epilogue-wuxia-v1.png`

Beat: She chooses the side of the living land and its people.

Prompt:

```text
Wide story-banner illustration. Epilogue scene after the storm. Cô Sử Tiên sits or stands near an open window writing by lamplight while outside the rain has stopped and ordinary homes glow warmly again. Nearby rooftops, riverbank, kitchen smoke, and sleeping neighborhoods suggest the people she has chosen to defend. Her clothing is simple and mature now, scholar robe layered with a plain shawl, hair in a loose low bun, expression peaceful but committed. The whole image should feel like relief, promise, and tenderness after struggle. Bright night with warm lantern gold, blue rain-washed sky, and gentle paper texture.
```

## Beast master portraits for future reuse

These are not panel images. These are clean source illustrations for unlock screens, promo, and possible cutouts.

### `assets/images/story/voi-chin-nga-wuxia-master-v1.png`

```text
Full-body Voi chín ngà in bright wuxia painting cartoon style, noble sacred elephant with nine readable tusks, child-friendly face, mountain-earth motifs, ivory-gray body with subtle jade and gold ornaments, simple bright background, clean silhouette for future cutout, no rider, no text, no realism.
```

### `assets/images/story/ga-chin-cua-wuxia-master-v1.png`

```text
Full-body Gà chín cựa in bright wuxia painting cartoon style, proud magical rooster with nine readable spurs, white head accents, layered red-gold-celadon feathers, clean silhouette, simple bright background for future cutout, child-friendly sacred beast, no realism.
```

### `assets/images/story/ngua-chin-hong-mao-wuxia-master-v1.png`

```text
Full-body Ngựa chín hồng mao in bright wuxia painting cartoon style, noble swift horse with vivid red mane and tail, sky-wind motifs, gold and jade harness details, clean silhouette, simple bright background for future cutout, child-friendly heroic energy, no realism.
```

## HUD icon prompts if dedicated icons are needed

If we do not derive icons from the master portraits, use these.

### `assets/images/story/voi-chin-nga-hud-icon-wuxia-v1.png`

```text
Square HUD icon, close-up of Voi chín ngà head and upper tusks, bright wuxia painting cartoon, simplified shapes, bold outline, readable at small size, transparent-friendly composition, child-friendly, no text.
```

### `assets/images/story/ga-chin-cua-hud-icon-wuxia-v1.png`

```text
Square HUD icon, close-up of Gà chín cựa head, chest, and key spurs, bright wuxia painting cartoon, simplified shapes, bold outline, readable at small size, child-friendly, no text.
```

### `assets/images/story/ngua-chin-hong-mao-hud-icon-wuxia-v1.png`

```text
Square HUD icon, close-up of Ngựa chín hồng mao head with flowing red mane, bright wuxia painting cartoon, simplified shapes, bold outline, readable at small size, child-friendly, no text.
```

## Generation order

1. Intro pages `01-08`
2. Set intros
3. Set win screens
4. Set lose screens
5. Unlock scenes
6. Epilogue
7. Beast master portraits
8. HUD icons only if needed
