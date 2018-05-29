function init()

	loadLevel( "data/level/level.hgr" )

	-- animate level object "crate_4" by loading
	-- animations from "data/level/level_boxanims.hgr"
	-- and then applying animation of object "crate_circling" from that scene
	playAnim( "data/level/level_boxanims.hgr:crate_circling", "crate_4" )
	
end
