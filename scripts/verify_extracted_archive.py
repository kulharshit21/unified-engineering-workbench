import zipfile
import os
import shutil
import subprocess
import time

def verify_extraction():
    tmp_dir = os.path.join(os.environ['TEMP'], 'CleanWorkbenchTest')
    if os.path.exists(tmp_dir):
        shutil.rmtree(tmp_dir, ignore_errors=True)
    os.makedirs(tmp_dir, exist_ok=True)

    zip_path = os.path.abspath('release/CandidateID_Workbench.zip')
    print(f"Testing extraction from: {zip_path}")
    print(f"Target scratch directory: {tmp_dir}")

    t0 = time.perf_counter()
    with zipfile.ZipFile(zip_path, 'r') as z:
        z.extractall(tmp_dir)
    t1 = time.perf_counter()
    print(f"[PASS] Successfully extracted {len(os.listdir(tmp_dir))} top-level entries in {t1 - t0:.2f} s")

    # Verify Launch.exe
    launcher = os.path.join(tmp_dir, 'Launch.exe')
    if not os.path.exists(launcher):
        print(f"[FAIL] Launch.exe missing at {launcher}")
        return False
    print(f"[PASS] Found Launch.exe ({os.path.getsize(launcher)} bytes)")

    # Verify WorkbenchApp.exe
    app_exe = os.path.join(tmp_dir, 'bin', 'WorkbenchApp.exe')
    if not os.path.exists(app_exe):
        print(f"[FAIL] WorkbenchApp.exe missing at {app_exe}")
        return False
    print(f"[PASS] Found bin/WorkbenchApp.exe ({os.path.getsize(app_exe)} bytes)")

    # Run WorkbenchApp.exe --smoke-test
    print("Executing out-of-tree: bin/WorkbenchApp.exe --smoke-test")
    p = subprocess.run([app_exe, '--smoke-test'], cwd=os.path.join(tmp_dir, 'bin'), capture_output=True, text=True)
    print(f"WorkbenchApp returncode: {p.returncode}")
    print(f"WorkbenchApp stdout: {p.stdout.strip()}")
    if p.returncode != 0:
        print(f"[FAIL] WorkbenchApp failed with code {p.returncode}: {p.stderr}")
        return False

    # Run Launch.exe --smoke-test
    print("Executing out-of-tree: Launch.exe --smoke-test")
    p2 = subprocess.run([launcher, '--smoke-test'], cwd=tmp_dir, capture_output=True, text=True)
    print(f"Launch.exe returncode: {p2.returncode}")
    if p2.returncode != 0:
        print(f"[FAIL] Launch.exe failed with code {p2.returncode}")
        return False

    # Check repository_bundle.bundle
    bundle = os.path.join(tmp_dir, 'repository_bundle.bundle')
    if not os.path.exists(bundle):
        print(f"[FAIL] repository_bundle.bundle missing")
        return False
    print(f"[PASS] Found repository_bundle.bundle ({os.path.getsize(bundle)} bytes)")

    # Verify bundle using git
    res = subprocess.run(['git', 'bundle', 'verify', bundle], capture_output=True, text=True)
    print(f"Git bundle verify returncode: {res.returncode}")
    if res.returncode == 0:
        print("[PASS] Offline Git repository bundle verified successfully.")
    else:
        print(f"[FAIL] Git bundle invalid: {res.stderr}")
        return False

    # Cleanup scratch
    shutil.rmtree(tmp_dir, ignore_errors=True)
    print("[PASS] Cleaned up temporary test directory.")
    print("ALL CLEAN ARCHIVE VERIFICATION CHECKS PASSED.")
    return True

if __name__ == '__main__':
    verify_extraction()
