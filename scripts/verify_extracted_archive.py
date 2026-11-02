import zipfile
import os
import shutil
import tempfile
import atexit
import subprocess
import time
import sys

def verify_extraction():
    candidate_id = sys.argv[1] if len(sys.argv) > 1 else 'CandidateID'
    # Use a unique scratch directory; never remove a fixed user-visible
    # %TEMP% path that may contain unrelated files.
    tmp_dir = tempfile.mkdtemp(prefix='CleanWorkbenchTest_')
    # Clean up even when a validation branch returns early.
    atexit.register(shutil.rmtree, tmp_dir, ignore_errors=True)

    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    zip_path = os.path.join(repo_root, 'release', f'{candidate_id}_Workbench.zip')
    print(f"Testing extraction from: {zip_path}")
    print(f"Target scratch directory: {tmp_dir}")

    t0 = time.perf_counter()
    try:
        with zipfile.ZipFile(zip_path, 'r') as z:
            if z.testzip() is not None:
                print("[FAIL] ZIP integrity check failed.")
                return False
            for entry in z.infolist():
                # PowerShell ZIPs use Windows separators, even on other hosts.
                relative = entry.filename.replace('\\', '/')
                target = os.path.realpath(os.path.join(tmp_dir, relative))
                if os.path.commonpath([os.path.realpath(tmp_dir), target]) != os.path.realpath(tmp_dir):
                    print("[FAIL] Unsafe ZIP entry path: " + entry.filename)
                    return False
                if entry.is_dir() or relative.endswith('/'):
                    os.makedirs(target, exist_ok=True)
                else:
                    os.makedirs(os.path.dirname(target), exist_ok=True)
                    with z.open(entry) as src, open(target, 'wb') as dst:
                        shutil.copyfileobj(src, dst)
    except (OSError, zipfile.BadZipFile, ValueError) as exc:
        print("[FAIL] ZIP extraction error: " + str(exc))
        return False
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
    p = subprocess.run([app_exe, '--smoke-test'], cwd=os.path.join(tmp_dir, 'bin'), capture_output=True, text=True, timeout=60)
    print(f"WorkbenchApp returncode: {p.returncode}")
    print(f"WorkbenchApp stdout: {p.stdout.strip()}")
    if p.returncode != 0:
        print(f"[FAIL] WorkbenchApp failed with code {p.returncode}: {p.stderr}")
        return False

    # Run Launch.exe --smoke-test
    print("Executing out-of-tree: Launch.exe --smoke-test")
    p2 = subprocess.run([launcher, '--smoke-test'], cwd=tmp_dir, capture_output=True, text=True, timeout=60)
    print(f"Launch.exe returncode: {p2.returncode}")
    if p2.returncode != 0:
        print(f"[FAIL] Launch.exe failed with code {p2.returncode}")
        return False

    # Verify the checksum against the exact archive being inspected.
    hash_path = os.path.join(repo_root, 'release', f'{candidate_id}_Workbench.sha256')
    if not os.path.isfile(hash_path):
        print('[FAIL] SHA-256 sidecar file missing.')
        return False
    import hashlib
    with open(hash_path, encoding='ascii') as hfile:
        recorded = hfile.read().strip().split()
    if len(recorded) != 2 or recorded[1] != os.path.basename(zip_path):
        print('[FAIL] SHA-256 sidecar filename or format mismatch.')
        return False
    digest = hashlib.sha256()
    with open(zip_path, 'rb') as source:
        for block in iter(lambda: source.read(1024 * 1024), b''):
            digest.update(block)
    if digest.hexdigest().lower() != recorded[0].lower():
        print('[FAIL] SHA-256 digest mismatch.')
        return False
    print('[PASS] SHA-256 sidecar matches archive bytes.')

    # Verify checksum sidecar against the exact archive under test.
    import hashlib
    hash_path = os.path.join(repo_root, 'release', f'{candidate_id}_Workbench.sha256')
    if not os.path.isfile(hash_path):
        print('[FAIL] SHA-256 sidecar is missing.')
        return False
    try:
        with open(hash_path, encoding='ascii') as sidecar:
            recorded = sidecar.read().strip().split()
    except (OSError, UnicodeError) as exc:
        print(f'[FAIL] Unable to read SHA-256 sidecar: {exc}')
        return False
    if len(recorded) != 2 or recorded[1] != os.path.basename(zip_path):
        print('[FAIL] SHA-256 sidecar format or filename mismatch.')
        return False
    digest = hashlib.sha256()
    with open(zip_path, 'rb') as archive_stream:
        for chunk in iter(lambda: archive_stream.read(1024 * 1024), b''):
            digest.update(chunk)
    if digest.hexdigest().lower() != recorded[0].lower():
        print('[FAIL] SHA-256 digest mismatch.')
        return False
    print('[PASS] SHA-256 checksum verified.')

    # Check essential directories and files
    required_entries = ['data', 'docs', 'src', 'tests', 'results', 'CMakeLists.txt', 'CMakePresets.json', 'README.md', 'FINAL_SUBMISSION_GATE.md']
    for entry in required_entries:
        path = os.path.join(tmp_dir, entry)
        if not os.path.exists(path):
            print(f"[FAIL] Required deliverable entry missing: {entry}")
            return False
        print(f"[PASS] Found required deliverable entry: {entry}")

    # Check for handwritten appendix scan
    appendix_pdf = os.path.join(tmp_dir, 'docs', 'appendix', 'HANDWRITTEN_APPENDIX_SCAN.pdf')
    if os.path.exists(appendix_pdf):
        print(f"[PASS] Found handwritten appendix scan: docs/appendix/HANDWRITTEN_APPENDIX_SCAN.pdf ({os.path.getsize(appendix_pdf)} bytes)")
    else:
        print(f"[INFO] Handwritten appendix scan docs/appendix/HANDWRITTEN_APPENDIX_SCAN.pdf not present in archive (pending candidate scan).")

    # Check repository_bundle.bundle
    bundle = os.path.join(tmp_dir, 'repository_bundle.bundle')
    if not os.path.exists(bundle):
        print(f"[FAIL] repository_bundle.bundle missing")
        return False
    print(f"[PASS] Found repository_bundle.bundle ({os.path.getsize(bundle)} bytes)")

    # Verify bundle using git
    # Verify without relying on the caller's working directory or Git state.
    scratch_repo = os.path.join(tmp_dir, '_verification_git')
    os.makedirs(scratch_repo, exist_ok=True)
    setup = subprocess.run(['git', 'init', '--quiet', scratch_repo], capture_output=True, text=True, timeout=30)
    if setup.returncode != 0:
        print(f'[FAIL] Could not prepare scratch repository: {setup.stderr}')
        return False
    res = subprocess.run(['git', 'bundle', 'verify', bundle], cwd=scratch_repo, capture_output=True, text=True, timeout=30)
    print(f"Git bundle verify returncode: {res.returncode}")
    if res.returncode == 0:
        print("[PASS] Offline Git repository bundle verified successfully.")
    else:
        print(f"[FAIL] Git bundle invalid: {res.stderr}")
        return False

    # Cleanup only our uniquely-created scratch directory.
    shutil.rmtree(tmp_dir, ignore_errors=True)
    print("[PASS] Cleaned up temporary test directory.")
    print("ALL CLEAN ARCHIVE VERIFICATION CHECKS PASSED.")
    return True

if __name__ == '__main__':
    success = verify_extraction()
    sys.exit(0 if success else 1)
