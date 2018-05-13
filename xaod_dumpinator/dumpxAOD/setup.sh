# setup ATLAS
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'

echo "=== running setupATLAS ==="
setupATLAS -q
echo "=== running asetup ==="
asetup AnalysisBase,21.2.29
