--
-- Exports zax source data
--
-- jk 3/12/2005
--
-- Files:
--     Sources file name filter, can contain wildcards.
--
-- Output:
--     Receives exported files, name must end with \ if directory.
--

fn exportFiles srcpath dstpath =
(
	maxfiles = getFiles srcpath
	for maxfile in maxfiles do 
	(
		ok = loadMaxFile maxfile useFileUnits:true
		if ( ok ) do
		(
			srcname = getFilenameFile maxfile
			fname = dstpath + srcname + ".hgr"
			exportFile fname #noPrompt
		)
	)
)

utility HgrBatchExport "hgr batch export"
(
	edittext sourcepath "Files" text:"\\Documents and Settings\\jani\\My Documents\\projects\\zaxdemo\\source_data\\"
	edittext outputpath "Output" text:"\\Documents and Settings\\jani\\My Documents\\projects\\zaxdemo\\data\\"
	button doitbtn "Batch Export"

	on doitbtn pressed do 
	(
		src = sourcepath.text
		dst = outputpath.text
		exportFiles (src+"anim\\zax\\*.max") (dst+"anim\\zax\\")
		exportFiles (src+"zax\\*.max") (dst+"zax\\")
		exportFiles (src+"worker_bot\\*.max") (dst+"worker_bot\\")
		exportFiles (src+"lasergun\\*.max") (dst+"lasergun\\")
		exportFiles (src+"glow_ball_coin\\*.max") (dst+"glow_ball_coin\\")
	)
)
