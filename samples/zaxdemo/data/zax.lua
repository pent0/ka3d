function init()
	name = "zax"

	setSkinnedMesh( "data/chars/zax/zax_mesh.hgr" );

	loadCharacterAnimation( "STAND", "data/anim/zax/idle_stand.hgr", "REPEAT" );
	loadCharacterAnimation( "WALK_FORWARD", "data/anim/zax/walk_frwd.hgr", "REPEAT" );
	loadCharacterAnimation( "JUMP_UP", "data/anim/zax/jump_up.hgr", "STOP" );
	loadCharacterAnimation( "JUMP_OUT", "data/anim/zax/jump_out.hgr", "STOP" );
	loadCharacterAnimation( "STEP_LEFT", "data/anim/zax/sidestep_left.hgr", "REPEAT" );
	loadCharacterAnimation( "STEP_RIGHT", "data/anim/zax/sidestep_right.hgr", "REPEAT" );
	loadCharacterAnimation( "IDLE1", "data/anim/zax/idle_look_around.hgr", "REPEAT" );
	loadCharacterAnimation( "IDLE2", "data/anim/zax/idle_stretch.hgr", "REPEAT" );

	loadUpperBodyAnimation( "AIM", "DOWN", "data/anim/zax/gun_down.hgr", "STOP" );
	loadUpperBodyAnimation( "AIM", "FRONT", "data/anim/zax/gun_front.hgr", "STOP" );
	loadUpperBodyAnimation( "AIM", "UP", "data/anim/zax/gun_up.hgr", "STOP" );
	loadUpperBodyAnimation( "SHOOT", "DOWN", "data/anim/zax/gun_down_shoot.hgr", "STOP" );
	loadUpperBodyAnimation( "SHOOT", "FRONT", "data/anim/zax/gun_front_shoot.hgr", "STOP" );
	loadUpperBodyAnimation( "SHOOT", "UP", "data/anim/zax/gun_up_shoot.hgr", "STOP" );

	-- set player global; after this you can refer to player character anywhere using _G.player global var
	-- from code you would do the same using luastate->globals().setTable( "player", player );
	_G.player = this
end
