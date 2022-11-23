-- GTT level from QJ, restored

levelData = {
  financialTarget = 16500,
  timeLimit = 300
}

playerState = {
  cash = 70
}



-- some helpful building definitions

shopLayout = {
  {a=0,b=0,}, {a=1,b=0,}, {a=2,b=0,}, {a=3,b=0,}, {a=4,b=0,}, {a=5,b=0,},
  {a=0,b=1,}, {a=1,b=1,}, {a=2,b=1,}, {a=3,b=1,}, {a=4,b=1,}, {a=5,b=1,},
  {a=0,b=2,}, {a=1,b=2,}, {a=2,b=2,}, {a=3,b=2,}, {a=4,b=2,}, {a=5,b=2,},
  {a=0,b=3,}, {a=1,b=3,}, {a=2,b=3,}, {a=3,b=3,}, {a=4,b=3,}, {a=5,b=3,},
                          {a=2,b=4,}, {a=3,b=4,},
}
shopDoors = {
  {a=2,b=4,}, {a=2,b=5,},
  {a=3,b=4,}, {a=3,b=5,},
}
shopLayout__flipped = {
                          {a=2,b=0,}, {a=3,b=0,},
  {a=0,b=1,}, {a=1,b=1,}, {a=2,b=1,}, {a=3,b=1,}, {a=4,b=1,}, {a=5,b=1,},
  {a=0,b=2,}, {a=1,b=2,}, {a=2,b=2,}, {a=3,b=2,}, {a=4,b=2,}, {a=5,b=2,},
  {a=0,b=3,}, {a=1,b=3,}, {a=2,b=3,}, {a=3,b=3,}, {a=4,b=3,}, {a=5,b=3,},
  {a=0,b=4,}, {a=1,b=4,}, {a=2,b=4,}, {a=3,b=4,}, {a=4,b=4,}, {a=5,b=4,},
}
shopDoors__flipped = {
  {a=2,b=0,}, {a=2,b=-1,},
  {a=3,b=0,}, {a=3,b=-1,},
}
houseLayout = {
  {a=0,b=0,}, {a=1,b=0,}, {a=2,b=0,}, {a=3,b=0,}, {a=4,b=0,},
  {a=0,b=1,}, {a=1,b=1,}, {a=2,b=1,}, {a=3,b=1,}, {a=4,b=1,},
  {a=0,b=2,}, {a=1,b=2,}, {a=2,b=2,}, {a=3,b=2,}, {a=4,b=2,},
  {a=0,b=3,}, {a=1,b=3,}, {a=2,b=3,}, {a=3,b=3,}, {a=4,b=3,},
                          {a=2,b=4,},
}
houseDoors = {
  {a=2,b=4,}, {a=2,b=5,},
}
houseLayout__flipped = {
                          {a=2,b=0,},
  {a=0,b=1,}, {a=1,b=1,}, {a=2,b=1,}, {a=3,b=1,}, {a=4,b=1,},
  {a=0,b=2,}, {a=1,b=2,}, {a=2,b=2,}, {a=3,b=2,}, {a=4,b=2,},
  {a=0,b=3,}, {a=1,b=3,}, {a=2,b=3,}, {a=3,b=3,}, {a=4,b=3,},
  {a=0,b=4,}, {a=1,b=4,}, {a=2,b=4,}, {a=3,b=4,}, {a=4,b=4,},
}
houseDoors__flipped = {
  {a=2,b=0,}, {a=2,b=-1,},
}
houseLayout__r90 = {
              {a=1,b=0,}, {a=2,b=0,}, {a=3,b=0,}, {a=4,b=0,},
              {a=1,b=1,}, {a=2,b=1,}, {a=3,b=1,}, {a=4,b=1,},
  {a=0,b=2,}, {a=1,b=2,}, {a=2,b=2,}, {a=3,b=2,}, {a=4,b=2,},
              {a=1,b=3,}, {a=2,b=3,}, {a=3,b=3,}, {a=4,b=3,},
              {a=1,b=4,}, {a=2,b=4,}, {a=3,b=4,}, {a=4,b=4,},
}
houseDoors__r90 = {
  {a=0,b=2,}, {a=-1,b=2,},
}
shopLayout__r90 = {
  {a=0,b=0,}, {a=1,b=0,}, {a=2,b=0,}, {a=3,b=0,},
  {a=0,b=1,}, {a=1,b=1,}, {a=2,b=1,}, {a=3,b=1,},
  {a=0,b=2,}, {a=1,b=2,}, {a=2,b=2,}, {a=3,b=2,}, {a=4,b=2,},
  {a=0,b=3,}, {a=1,b=3,}, {a=2,b=3,}, {a=3,b=3,}, {a=4,b=3,},
  {a=0,b=4,}, {a=1,b=4,}, {a=2,b=4,}, {a=3,b=4,},
  {a=0,b=5,}, {a=1,b=5,}, {a=2,b=5,}, {a=3,b=5,},
}
shopDoors__r90 = {
  {a=4,b=2,}, {a=5,b=2,},
  {a=4,b=3,}, {a=5,b=3,},
}
shopLayout__r270 = {
              {a=1,b=0,}, {a=2,b=0,}, {a=3,b=0,}, {a=4,b=0,},
              {a=1,b=1,}, {a=2,b=1,}, {a=3,b=1,}, {a=4,b=1,},
  {a=0,b=2,}, {a=1,b=2,}, {a=2,b=2,}, {a=3,b=2,}, {a=4,b=2,},
  {a=0,b=3,}, {a=1,b=3,}, {a=2,b=3,}, {a=3,b=3,}, {a=4,b=3,},
              {a=1,b=4,}, {a=2,b=4,}, {a=3,b=4,}, {a=4,b=4,},
              {a=1,b=5,}, {a=2,b=5,}, {a=3,b=5,}, {a=4,b=5,},
}
shopDoors__r270 = {
  {a=0,b=2,}, {a=-1,b=2,},
  {a=0,b=3,}, {a=-1,b=3,},
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
      rct = { position={a=2,b=2,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1000,
      destroyed = false,
    },
    {
      rct = { position={a=9,b=2,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = houseDoors,
      uid = 1001,
      destroyed = false,
    },
    {
      rct = { position={a=16,b=2,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1002,
      destroyed = false,
    },
    {
      rct = { position={a=22,b=2,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1003,
      destroyed = false,
    },
    {
      rct = { position={a=29,b=2,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1004,
      destroyed = false,
    },
    {
      rct = { position={a=35,b=2,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1005,
      destroyed = false,
    },


    -- Row of houses facing the previous row
    {
      rct = { position={a=2,b=10,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1006,
      destroyed = false
    },
    {
      rct = { position={a=8,b=10,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1007,
      destroyed = false
    },
    {
      rct = { position={a=15,b=10,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1008,
      destroyed = false
    },
    {
      rct = { position={a=21,b=10,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1009,
      destroyed = false
    },
    {
      rct = { position={a=28,b=10,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1010,
      destroyed = false
    },
    {
      rct = { position={a=34,b=10,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1011,
      destroyed = false
    },

    -- WALL
    {
      rct = { position={a=2,b=16,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = {
        {a=0,b=0,}, {a=1,b=0,}, {a=2,b=0,}, {a=3,b=0,}, {a=4,b=0,}, {a=5,b=0,}, {a=6,b=0,}, {a=7,b=0,}, {a=8,b=0,}, {a=9,b=0,}, {a=10,b=0,},
        {a=0,b=1,},                                                                                                             {a=10,b=1,},
        {a=0,b=2,},                                                                                                             {a=10,b=2,},
        {a=0,b=3,}, {a=1,b=3,}, {a=2,b=3,},                                                                                     {a=10,b=3,},
                                                                                                                                {a=10,b=4,},
        {a=0,b=5,}, {a=1,b=5,}, {a=2,b=5,},                                                                                     {a=10,b=5,},
        {a=0,b=6,},                                                                                                             {a=10,b=6,},
        {a=0,b=7,},                                                                                                             {a=10,b=7,},
        {a=0,b=8,}, {a=1,b=8,}, {a=2,b=8,}, {a=3,b=8,}, {a=4,b=8,}, {a=5,b=8,}, {a=6,b=8,}, {a=7,b=8,}, {a=8,b=8,}, {a=9,b=8,}, {a=10,b=8,},

        {a=12,b=0,}, {a=13,b=0,}, {a=14,b=0,}, {a=15,b=0,}, {a=16,b=0,}, {a=17,b=0,}, {a=18,b=0,}, {a=19,b=0,}, {a=20,b=0,}, {a=21,b=0,}, {a=22,b=0,}, {a=23,b=0,}, {a=24,b=0,}, {a=25,b=0,}, {a=26,b=0,}, {a=27,b=0,}, {a=28,b=0,},
        {a=12,b=1,},                                                                                            {a=20,b=1,},                                                                                            {a=28,b=1,},
        {a=12,b=2,},                                                                                            {a=20,b=2,},                                                                                            {a=28,b=2,},
        {a=12,b=3,},                                                                                            {a=20,b=3,},                                                                                            {a=28,b=3,},
        {a=12,b=4,},                                                                                            {a=20,b=4,},                                                                                            {a=28,b=4,},
        {a=12,b=5,},                                                                                            {a=20,b=5,},                                                                                            {a=28,b=5,},
        {a=12,b=6,},                                                                                            {a=20,b=6,},                                                                                            {a=28,b=6,},
        {a=12,b=7,},              {a=14,b=7,},                                        {a=18,b=7,},              {a=20,b=7,},              {a=22,b=7,},                                        {a=26,b=7,},              {a=28,b=7,},
        {a=12,b=8,}, {a=13,b=8,}, {a=14,b=8,},                                        {a=18,b=8,}, {a=19,b=8,}, {a=20,b=8,}, {a=21,b=8,}, {a=22,b=8,},                                        {a=26,b=8,}, {a=27,b=8,}, {a=28,b=8,},

        {a=30,b=0,}, {a=31,b=0,}, {a=32,b=0,}, {a=33,b=0,}, {a=34,b=0,}, {a=35,b=0,}, {a=36,b=0,}, {a=37,b=0,}, {a=38,b=0,},
        {a=30,b=1,},                                                                                            {a=38,b=1,},
        {a=30,b=2,},                                                                                            {a=38,b=2,},
        {a=30,b=3,},                                                                                            {a=38,b=3,},
        {a=30,b=4,},
        {a=30,b=5,},
        {a=30,b=6,},                                                                                            {a=38,b=6,},
        {a=30,b=7,},                                                                                            {a=38,b=7,},
        {a=30,b=8,},                                                                                            {a=38,b=8,},
        {a=30,b=9,}, {a=31,b=9,}, {a=32,b=9,}, {a=33,b=9,}, {a=34,b=9,}, {a=35,b=9,}, {a=36,b=9,}, {a=37,b=9,}, {a=38,b=9,},
      },
      doors = { },
      uid = 1012,
      destroyed = false,
    },


    -- Walled-in houses
    {
      rct = { position={a=6,b=18,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__r90,
      doors = houseDoors__r90,
      uid = 1013,
      destroyed = false,
    },
    {
      rct = { position={a=16,b=18,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1014,
      destroyed = false,
    },
    {
      rct = { position={a=24,b=18,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1015,
      destroyed = false,
    },
    {
      rct = { position={a=34,b=18,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout__r90,
      doors = shopDoors__r90,
      uid = 1016,
      destroyed = false,
    },


    -- Bottom row of buildings

    {
      rct = { position={a=2,b=31,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1017,
      destroyed = false
    },
    {
      rct = { position={a=9,b=32,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1018,
      destroyed = false
    },
    {
      rct = { position={a=17,b=31,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1019,
      destroyed = false
    },
    {
      rct = { position={a=25,b=32,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout,
      doors = houseDoors,
      uid = 1020,
      destroyed = false
    },
    {
      rct = { position={a=32,b=31,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1021,
      destroyed = false
    },
    {
      rct = { position={a=39,b=32,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1022,
      destroyed = false
    },
    {
      rct = { position={a=48,b=8,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__r270,
      doors = shopDoors__r270,
      uid = 1023,
      destroyed = false
    },


    -- Small rightward ganglion of shops
    {
      rct = { position={a=44,b=2,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1024,
      destroyed = false
    },
    {
      rct = { position={a=48,b=8,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout__r270,
      doors = shopDoors__r270,
      uid = 1025,
      destroyed = false
    },
    {
      rct = { position={a=44,b=15,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = houseLayout__flipped,
      doors = houseDoors__flipped,
      uid = 1026,
      destroyed = false
    },
    {
      rct = { position={a=48,b=22,}, size={width=0,height=0} },
      ["type"] = "barber",
      groundplan = shopLayout,
      doors = shopDoors,
      uid = 1027,
      destroyed = false
    },
    {
      rct = { position={a=48,b=31,}, size={width=0,height=0} },
      ["type"] = "pieshop",
      groundplan = shopLayout__flipped,
      doors = shopDoors__flipped,
      uid = 1028,
      destroyed = false
    },


    -- wall the 2nd
    {
      rct = { position={a=42,b=2,}, size={width=0,height=0} },
      ["type"] = "home",
      groundplan = {
        {a=0,b=0,},
        {a=0,b=1,},
        {a=0,b=2,},
        {a=0,b=3,},
        {a=0,b=4,},
        {a=0,b=5,},

        {a=0,b=7,},
        {a=0,b=8,},
        {a=0,b=9,},
        {a=0,b=10,},
        {a=0,b=11,},
        {a=0,b=12,},
      },
      doors = { },
      uid = 1029,
      destroyed = false,
    },
  }
}
