function init()
	_G.coinsCollected = 0
	setMeshFile( "data/glow_ball_coin/glow_ball_coin.hgr" )
end

function signalPlayer()
	_G.coinsCollected = _G.coinsCollected + 1
	removeMesh()
end
