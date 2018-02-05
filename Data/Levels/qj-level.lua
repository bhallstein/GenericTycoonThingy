-- GTT level from QJ, restored

levelData = {
  monetaryTarget = 1200,
  timeLimit = 300
}

playerState = {
  balance = 70
}



-- some helpful building definitions

shopLayout = {
  {x=0,y=0,a=0,b=0}, {x=1,y=0,a=0,b=0}, {x=2,y=0,a=0,b=0}, {x=3,y=0,a=0,b=0}, {x=4,y=0,a=0,b=0}, {x=5,y=0,a=0,b=0},
  {x=0,y=1,a=0,b=0}, {x=1,y=1,a=0,b=0}, {x=2,y=1,a=0,b=0}, {x=3,y=1,a=0,b=0}, {x=4,y=1,a=0,b=0}, {x=5,y=1,a=0,b=0},
  {x=0,y=2,a=0,b=0}, {x=1,y=2,a=0,b=0}, {x=2,y=2,a=0,b=0}, {x=3,y=2,a=0,b=0}, {x=4,y=2,a=0,b=0}, {x=5,y=2,a=0,b=0},
  {x=0,y=3,a=0,b=0}, {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0}, {x=3,y=3,a=0,b=0}, {x=4,y=3,a=0,b=0}, {x=5,y=3,a=0,b=0},
                                        {x=2,y=4,a=0,b=0}, {x=3,y=4,a=0,b=0},
}
shopDoors = {
  {x=2,y=4,a=0,b=0}, {x=2,y=5,a=0,b=0},
  {x=3,y=4,a=0,b=0}, {x=3,y=5,a=0,b=0},
}
shopLayout__flipped = {
                                        {x=2,y=0,a=0,b=0}, {x=3,y=0,a=0,b=0},
  {x=0,y=1,a=0,b=0}, {x=1,y=1,a=0,b=0}, {x=2,y=1,a=0,b=0}, {x=3,y=1,a=0,b=0}, {x=4,y=1,a=0,b=0}, {x=5,y=1,a=0,b=0},
  {x=0,y=2,a=0,b=0}, {x=1,y=2,a=0,b=0}, {x=2,y=2,a=0,b=0}, {x=3,y=2,a=0,b=0}, {x=4,y=2,a=0,b=0}, {x=5,y=2,a=0,b=0},
  {x=0,y=3,a=0,b=0}, {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0}, {x=3,y=3,a=0,b=0}, {x=4,y=3,a=0,b=0}, {x=5,y=3,a=0,b=0},
  {x=0,y=4,a=0,b=0}, {x=1,y=4,a=0,b=0}, {x=2,y=4,a=0,b=0}, {x=3,y=4,a=0,b=0}, {x=4,y=4,a=0,b=0}, {x=5,y=4,a=0,b=0},
}
shopDoors__flipped = {
  {x=2,y=0,a=0,b=0}, {x=2,y=-1,a=0,b=0},
  {x=3,y=0,a=0,b=0}, {x=3,y=-1,a=0,b=0},
}
houseLayout = {
  {x=0,y=0,a=0,b=0}, {x=1,y=0,a=0,b=0}, {x=2,y=0,a=0,b=0}, {x=3,y=0,a=0,b=0}, {x=4,y=0,a=0,b=0},
  {x=0,y=1,a=0,b=0}, {x=1,y=1,a=0,b=0}, {x=2,y=1,a=0,b=0}, {x=3,y=1,a=0,b=0}, {x=4,y=1,a=0,b=0},
  {x=0,y=2,a=0,b=0}, {x=1,y=2,a=0,b=0}, {x=2,y=2,a=0,b=0}, {x=3,y=2,a=0,b=0}, {x=4,y=2,a=0,b=0},
  {x=0,y=3,a=0,b=0}, {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0}, {x=3,y=3,a=0,b=0}, {x=4,y=3,a=0,b=0},
                                        {x=2,y=4,a=0,b=0},
}
houseDoors = {
  {x=2,y=4,a=0,b=0}, {x=2,y=5,a=0,b=0},
}
houseLayout__flipped = {
                                        {x=2,y=0,a=0,b=0},
  {x=0,y=1,a=0,b=0}, {x=1,y=1,a=0,b=0}, {x=2,y=1,a=0,b=0}, {x=3,y=1,a=0,b=0}, {x=4,y=1,a=0,b=0},
  {x=0,y=2,a=0,b=0}, {x=1,y=2,a=0,b=0}, {x=2,y=2,a=0,b=0}, {x=3,y=2,a=0,b=0}, {x=4,y=2,a=0,b=0},
  {x=0,y=3,a=0,b=0}, {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0}, {x=3,y=3,a=0,b=0}, {x=4,y=3,a=0,b=0},
  {x=0,y=4,a=0,b=0}, {x=1,y=4,a=0,b=0}, {x=2,y=4,a=0,b=0}, {x=3,y=4,a=0,b=0}, {x=4,y=4,a=0,b=0},
}
houseDoors__flipped = {
  {x=2,y=0,a=0,b=0}, {x=2,y=-1,a=0,b=0},
}
houseLayout__r90 = {
                     {x=1,y=0,a=0,b=0}, {x=2,y=0,a=0,b=0}, {x=3,y=0,a=0,b=0}, {x=4,y=0,a=0,b=0},
                     {x=1,y=1,a=0,b=0}, {x=2,y=1,a=0,b=0}, {x=3,y=1,a=0,b=0}, {x=4,y=1,a=0,b=0},
  {x=0,y=2,a=0,b=0}, {x=1,y=2,a=0,b=0}, {x=2,y=2,a=0,b=0}, {x=3,y=2,a=0,b=0}, {x=4,y=2,a=0,b=0},
                     {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0}, {x=3,y=3,a=0,b=0}, {x=4,y=3,a=0,b=0},
                     {x=1,y=4,a=0,b=0}, {x=2,y=4,a=0,b=0}, {x=3,y=4,a=0,b=0}, {x=4,y=4,a=0,b=0},
}
houseDoors__r90 = {
  {x=0,y=2,a=0,b=0}, {x=-1,y=2,a=0,b=0},
}
shopLayout__r90 = {
  {x=0,y=0,a=0,b=0}, {x=1,y=0,a=0,b=0}, {x=2,y=0,a=0,b=0}, {x=3,y=0,a=0,b=0},
  {x=0,y=1,a=0,b=0}, {x=1,y=1,a=0,b=0}, {x=2,y=1,a=0,b=0}, {x=3,y=1,a=0,b=0},
  {x=0,y=2,a=0,b=0}, {x=1,y=2,a=0,b=0}, {x=2,y=2,a=0,b=0}, {x=3,y=2,a=0,b=0}, {x=4,y=2,a=0,b=0},
  {x=0,y=3,a=0,b=0}, {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0}, {x=3,y=3,a=0,b=0}, {x=4,y=3,a=0,b=0},
  {x=0,y=4,a=0,b=0}, {x=1,y=4,a=0,b=0}, {x=2,y=4,a=0,b=0}, {x=3,y=4,a=0,b=0},
  {x=0,y=5,a=0,b=0}, {x=1,y=5,a=0,b=0}, {x=2,y=5,a=0,b=0}, {x=3,y=5,a=0,b=0},
}
shopDoors__r90 = {
  {x=4,y=2,a=0,b=0}, {x=5,y=2,a=0,b=0},
  {x=4,y=3,a=0,b=0}, {x=5,y=3,a=0,b=0},
}
shopLayout__r270 = {
                     {x=1,y=0,a=0,b=0}, {x=2,y=0,a=0,b=0}, {x=3,y=0,a=0,b=0}, {x=4,y=0,a=0,b=0},
                     {x=1,y=1,a=0,b=0}, {x=2,y=1,a=0,b=0}, {x=3,y=1,a=0,b=0}, {x=4,y=1,a=0,b=0},
  {x=0,y=2,a=0,b=0}, {x=1,y=2,a=0,b=0}, {x=2,y=2,a=0,b=0}, {x=3,y=2,a=0,b=0}, {x=4,y=2,a=0,b=0},
  {x=0,y=3,a=0,b=0}, {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0}, {x=3,y=3,a=0,b=0}, {x=4,y=3,a=0,b=0},
                     {x=1,y=4,a=0,b=0}, {x=2,y=4,a=0,b=0}, {x=3,y=4,a=0,b=0}, {x=4,y=4,a=0,b=0},
                     {x=1,y=5,a=0,b=0}, {x=2,y=5,a=0,b=0}, {x=3,y=5,a=0,b=0}, {x=4,y=5,a=0,b=0},
}
shopDoors__r270 = {
  {x=0,y=2,a=0,b=0}, {x=-1,y=2,a=0,b=0},
  {x=0,y=3,a=0,b=0}, {x=-1,y=3,a=0,b=0},
}




-- level contents

mapData = {
  width = 55,
  height = 45,

  units = { },
  furnishings = { },
  controllers = { },
  inactiveControllers = { },

  buildings = {

    -- Top row of houses
    {
      rct = { position={x=2,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1000,
      destroyed = false,
    },
    {
      rct = { position={x=9,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = houseDoors,
      uid = 1001,
      destroyed = false,
    },
    {
      rct = { position={x=16,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1002,
      destroyed = false,
    },
    {
      rct = { position={x=22,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1003,
      destroyed = false,
    },
    {
      rct = { position={x=29,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1004,
      destroyed = false,
    },
    {
      rct = { position={x=35,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1005,
      destroyed = false,
    },


    -- Row of houses facing the previous row
    {
      rct = { position={x=2,y=10,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1006,
      destroyed = false
    },
    {
      rct = { position={x=8,y=10,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1007,
      destroyed = false
    },
    {
      rct = { position={x=15,y=10,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1008,
      destroyed = false
    },
    {
      rct = { position={x=21,y=10,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1009,
      destroyed = false
    },
    {
      rct = { position={x=28,y=10,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1010,
      destroyed = false
    },
    {
      rct = { position={x=34,y=10,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1011,
      destroyed = false
    },

    -- WALL
    {
      rct = { position={x=2,y=16,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = {
        {x=0,y=0,a=0,b=0}, {x=1,y=0,a=0,b=0}, {x=2,y=0,a=0,b=0}, {x=3,y=0,a=0,b=0}, {x=4,y=0,a=0,b=0}, {x=5,y=0,a=0,b=0}, {x=6,y=0,a=0,b=0}, {x=7,y=0,a=0,b=0}, {x=8,y=0,a=0,b=0}, {x=9,y=0,a=0,b=0}, {x=10,y=0,a=0,b=0},
        {x=0,y=1,a=0,b=0},                                                                                                                                                                            {x=10,y=1,a=0,b=0},
        {x=0,y=2,a=0,b=0},                                                                                                                                                                            {x=10,y=2,a=0,b=0},
        {x=0,y=3,a=0,b=0}, {x=1,y=3,a=0,b=0}, {x=2,y=3,a=0,b=0},                                                                                                                                      {x=10,y=3,a=0,b=0},
                                                                                                                                                                                                      {x=10,y=4,a=0,b=0},
        {x=0,y=5,a=0,b=0}, {x=1,y=5,a=0,b=0}, {x=2,y=5,a=0,b=0},                                                                                                                                      {x=10,y=5,a=0,b=0},
        {x=0,y=6,a=0,b=0},                                                                                                                                                                            {x=10,y=6,a=0,b=0},
        {x=0,y=7,a=0,b=0},                                                                                                                                                                            {x=10,y=7,a=0,b=0},
        {x=0,y=8,a=0,b=0}, {x=1,y=8,a=0,b=0}, {x=2,y=8,a=0,b=0}, {x=3,y=8,a=0,b=0}, {x=4,y=8,a=0,b=0}, {x=5,y=8,a=0,b=0}, {x=6,y=8,a=0,b=0}, {x=7,y=8,a=0,b=0}, {x=8,y=8,a=0,b=0}, {x=9,y=8,a=0,b=0}, {x=10,y=8,a=0,b=0},

        {x=12,y=0,a=0,b=0}, {x=13,y=0,a=0,b=0}, {x=14,y=0,a=0,b=0}, {x=15,y=0,a=0,b=0}, {x=16,y=0,a=0,b=0}, {x=17,y=0,a=0,b=0}, {x=18,y=0,a=0,b=0}, {x=19,y=0,a=0,b=0}, {x=20,y=0,a=0,b=0}, {x=21,y=0,a=0,b=0}, {x=22,y=0,a=0,b=0}, {x=23,y=0,a=0,b=0}, {x=24,y=0,a=0,b=0}, {x=25,y=0,a=0,b=0}, {x=26,y=0,a=0,b=0}, {x=27,y=0,a=0,b=0}, {x=28,y=0,a=0,b=0},
        {x=12,y=1,a=0,b=0},                                                                                                                                             {x=20,y=1,a=0,b=0},                                                                                                                                             {x=28,y=1,a=0,b=0},
        {x=12,y=2,a=0,b=0},                                                                                                                                             {x=20,y=2,a=0,b=0},                                                                                                                                             {x=28,y=2,a=0,b=0},
        {x=12,y=3,a=0,b=0},                                                                                                                                             {x=20,y=3,a=0,b=0},                                                                                                                                             {x=28,y=3,a=0,b=0},
        {x=12,y=4,a=0,b=0},                                                                                                                                             {x=20,y=4,a=0,b=0},                                                                                                                                             {x=28,y=4,a=0,b=0},
        {x=12,y=5,a=0,b=0},                                                                                                                                             {x=20,y=5,a=0,b=0},                                                                                                                                             {x=28,y=5,a=0,b=0},
        {x=12,y=6,a=0,b=0},                                                                                                                                             {x=20,y=6,a=0,b=0},                                                                                                                                             {x=28,y=6,a=0,b=0},
        {x=12,y=7,a=0,b=0},                     {x=14,y=7,a=0,b=0},                                                             {x=18,y=7,a=0,b=0},                     {x=20,y=7,a=0,b=0},                     {x=22,y=7,a=0,b=0},                                                             {x=26,y=7,a=0,b=0},                     {x=28,y=7,a=0,b=0},
        {x=12,y=8,a=0,b=0}, {x=13,y=8,a=0,b=0}, {x=14,y=8,a=0,b=0},                                                             {x=18,y=8,a=0,b=0}, {x=19,y=8,a=0,b=0}, {x=20,y=8,a=0,b=0}, {x=21,y=8,a=0,b=0}, {x=22,y=8,a=0,b=0},                                                             {x=26,y=8,a=0,b=0}, {x=27,y=8,a=0,b=0}, {x=28,y=8,a=0,b=0},

        {x=30,y=0,a=0,b=0}, {x=31,y=0,a=0,b=0}, {x=32,y=0,a=0,b=0}, {x=33,y=0,a=0,b=0}, {x=34,y=0,a=0,b=0}, {x=35,y=0,a=0,b=0}, {x=36,y=0,a=0,b=0}, {x=37,y=0,a=0,b=0}, {x=38,y=0,a=0,b=0},
        {x=30,y=1,a=0,b=0},                                                                                                                                             {x=38,y=1,a=0,b=0},
        {x=30,y=2,a=0,b=0},                                                                                                                                             {x=38,y=2,a=0,b=0},
        {x=30,y=3,a=0,b=0},                                                                                                                                             {x=38,y=3,a=0,b=0},
        {x=30,y=4,a=0,b=0},
        {x=30,y=5,a=0,b=0},
        {x=30,y=6,a=0,b=0},                                                                                                                                             {x=38,y=7,a=0,b=0},
        {x=30,y=7,a=0,b=0},                                                                                                                                             {x=38,y=7,a=0,b=0},
        {x=30,y=8,a=0,b=0},                                                                                                                                             {x=38,y=8,a=0,b=0},
        {x=30,y=9,a=0,b=0}, {x=31,y=9,a=0,b=0}, {x=32,y=9,a=0,b=0}, {x=33,y=9,a=0,b=0}, {x=34,y=9,a=0,b=0}, {x=35,y=9,a=0,b=0}, {x=36,y=9,a=0,b=0}, {x=37,y=9,a=0,b=0}, {x=38,y=9,a=a,b=0},
      },
      doors = { },
      uid = 1012,
      destroyed = false,
    },


    -- Walled-in houses
    {
      rct = { position={x=6,y=18,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__r90,
      doors = houseDoors__r90,
      uid = 1013,
      destroyed = false,
    },
    {
      rct = { position={x=16,y=18,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1014,
      destroyed = false,
    },
    {
      rct = { position={x=24,y=18,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1015,
      destroyed = false,
    },
    {
      rct = { position={x=34,y=18,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout__r90,
      doors = shopDoors__r90,
      uid = 1016,
      destroyed = false,
    },


    -- Bottom row of buildings

    {
      rct = { position={x=2,y=31,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1017,
      destroyed = false
    },
    {
      rct = { position={x=9,y=32,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1018,
      destroyed = false
    },
    {
      rct = { position={x=17,y=31,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1019,
      destroyed = false
    },
    {
      rct = { position={x=25,y=32,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1020,
      destroyed = false
    },
    {
      rct = { position={x=32,y=31,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1021,
      destroyed = false
    },
    {
      rct = { position={x=39,y=32,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1022,
      destroyed = false
    },
    {
      rct = { position={x=48,y=8,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__r270,
      doors = shopDoors__r270,
      uid = 1023,
      destroyed = false
    },


    -- Small rightward ganglion of shops
    {
      rct = { position={x=44,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1024,
      destroyed = false
    },
    {
      rct = { position={x=48,y=8,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__r270,
      doors = shopDoors__r270,
      uid = 1025,
      destroyed = false
    },
    {
      rct = { position={x=44,y=15,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1026,
      destroyed = false
    },
    {
      rct = { position={x=48,y=22,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1027,
      destroyed = false
    },
    {
      rct = { position={x=48,y=31,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1028,
      destroyed = false
    },


    -- wall the 2nd
    {
      rct = { position={x=42,y=2,a=0,b=0}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = {
        {x=0,y=0,a=0,b=0},
        {x=0,y=1,a=0,b=0},
        {x=0,y=2,a=0,b=0},
        {x=0,y=3,a=0,b=0},
        {x=0,y=4,a=0,b=0},
        {x=0,y=5,a=0,b=0},

        {x=0,y=7,a=0,b=0},
        {x=0,y=8,a=0,b=0},
        {x=0,y=9,a=0,b=0},
        {x=0,y=10,a=0,b=0},
        {x=0,y=11,a=0,b=0},
        {x=0,y=12,a=0,b=0},
      },
      doors = { },
      uid = 1029,
      destroyed = false,
    },
  }
}



  -- spawnPoints = {
  --   { x = 4,  y = 13, name = "something" },
  --   { x = 18, y = 4, name = "something" },
  --   { x = 17, y = 13, name = "something" },
  --   { x = 31, y = 4, name = "something" },
  --   { x = 30, y = 13, name = "something" },
  --   { x = 8,  y = 20, name = "something" },
  --   { x = 18, y = 20, name = "something" },
  --   { x = 26, y = 20, name = "something" },
  --   { x = 46, y = 18, name = "something" },
  --   { x = 34, y = 34, name = "something" },
  --   { x = 27, y = 34, name = "something" },
  --   { x = 4,  y = 34, name = "something" }
  -- }



-- spawnPoints = {
--   {
--     name = "OffTheEdgeOfTheMap1EndNameHere",
--     x = 0,
--     y = 15,
--     -- rate = 70,
--     -- types = {
--     --   { Peasants, 5 },
--     --   { Judges, 8 },
--     -- },
--   },
--   {
--     name = "InAHouse",
--     x = 33,
--     y = 11,
--     -- rate = 80,
--     -- types = {
--     --   { Peasants, 5 },
--     --   { Judges, 8 },
--     -- },
--   },
--   {
--     name = "InAHouse1",
--     x = 33,
--     y = 21,
--     -- rate = 90,
--     -- types = {
--     --   { Peasants, 5 },
--     --   { Judges, 8 },
--     -- },
--   }
-- }
